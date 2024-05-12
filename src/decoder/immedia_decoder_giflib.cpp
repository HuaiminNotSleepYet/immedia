#include "immedia_decoder_giflib.h"

#include <limits>
#include <stdio.h>

#include "gif_lib.h"

#include "immedia_image.h"

struct GiflibDecoderContext
{
    GifFileType* Gif;
    bool         HasAlpha;

    uint8_t*     FrameBuffer;
    int          CurrentFrame;
};

static GiflibDecoderContext* GifRead(GifFileType* gif)
{
    if (!gif)
        return nullptr;
    if (DGifSlurp(gif) != GIF_OK)
    {
        DGifCloseFile(gif, nullptr);
        return nullptr;
    }

    GiflibDecoderContext* ctx = new GiflibDecoderContext();
    ctx->Gif = gif;

    for (size_t i = 0; i < gif->ImageCount && ctx->HasAlpha == false; i++)
    {
        const SavedImage& image = gif->SavedImages[i];
        for (size_t j = 0; j < image.ExtensionBlockCount; j++)
        {
            const ExtensionBlock& block = image.ExtensionBlocks[j];
            if (block.Function == GRAPHICS_EXT_FUNC_CODE)
            {
                ctx->HasAlpha = block.Bytes[4] != 0;
                break;
            }
        }
    }
    size_t frame_size = (size_t)gif->SWidth * gif->SHeight * (ctx->HasAlpha ? 4 : 3);
    ctx->FrameBuffer = new uint8_t[frame_size];
    memset(ctx->FrameBuffer, 0, frame_size);
    ctx->CurrentFrame = 0;
    return ctx;
}

static int GifFileInputFunc(GifFileType* gif, GifByteType* buf, int len)
{
    FILE* f = reinterpret_cast<FILE*>(gif->UserData);
    int l = static_cast<int>(fread(buf, 1, len, f));
    if (l < len)
    {
        fclose(f);
        gif->UserData = nullptr;
    }
    return l;
}

static void* CreateContextFromFile(void* f, size_t file_size)
{
    if (file_size > INT_MAX)
        return nullptr;
    GiflibDecoderContext* ctx = GifRead(DGifOpen(f, GifFileInputFunc, nullptr));
    if (ctx->Gif->UserData)
        fclose(reinterpret_cast<FILE*>(f));
    return ctx;
}

static int GifInputFunc(GifFileType* gif, GifByteType* buf, int len)
{
    const uint8_t** d = reinterpret_cast<const uint8_t**>(gif->UserData);
    int l = (d[0] + len >= d[1])
        ? (int)(d[1] - d[0])
        : len;
    memcpy(buf, d[0], l);
    d[0] += l;
    return l;
}

static void* CreateContextFromData(const uint8_t* data, size_t data_size)
{
    const uint8_t** d = new const uint8_t* [2] { data, data + data_size };
    return GifRead(DGifOpen(d, GifInputFunc, nullptr));
}

static void DeleteContext(void* context)
{
    GiflibDecoderContext* ctx = reinterpret_cast<GiflibDecoderContext*>(context);
    DGifCloseFile(ctx->Gif, nullptr);
    delete[] ctx->FrameBuffer;
    delete ctx;
}

static void GetInfo(void* context, int* width, int* height, ImMedia::PixelFormat* format, int* frame_count)
{
    GiflibDecoderContext* ctx = reinterpret_cast<GiflibDecoderContext*>(context);
    if (width)
        *width = ctx->Gif->SWidth;
    if (height)
        *height = ctx->Gif->SHeight;
    if (format)
        *format = ctx->HasAlpha ? ImMedia::PixelFormat::RGBA8888 : ImMedia::PixelFormat::RGB888;
    if (frame_count)
        *frame_count = ctx->Gif->ImageCount == 1 ? 0 : ctx->Gif->ImageCount;
}

static bool BeginReadFrame(void* context, uint8_t** pixels, int* delay_in_ms)
{
    GiflibDecoderContext* ctx = reinterpret_cast<GiflibDecoderContext*>(context);
    
    const SavedImage&    frame = ctx->Gif->SavedImages[ctx->CurrentFrame];
    GraphicsControlBlock graphic_block;

    DGifSavedExtensionToGCB(ctx->Gif, ctx->CurrentFrame, &graphic_block);

    for (size_t y = 0; y < frame.ImageDesc.Height; ++y)
    {
        for (size_t x = 0; x < frame.ImageDesc.Width; ++x)
        {
            const ColorMapObject* color_map = frame.ImageDesc.ColorMap ? frame.ImageDesc.ColorMap : ctx->Gif->SColorMap;
            uint8_t color_index = frame.RasterBits[x + y * frame.ImageDesc.Width];

            uint8_t* pixel = ctx->FrameBuffer + ((y + frame.ImageDesc.Top) * ctx->Gif->SWidth + (x + frame.ImageDesc.Left))  * (ctx->HasAlpha ? 4 : 3);

            if (!ctx->HasAlpha)
            {
                pixel[0] = color_map->Colors[color_index].Red;
                pixel[1] = color_map->Colors[color_index].Green;
                pixel[2] = color_map->Colors[color_index].Blue;
            }
            else if (color_index != graphic_block.TransparentColor) 
            {
                pixel[0] = color_map->Colors[color_index].Red;
                pixel[1] = color_map->Colors[color_index].Green;
                pixel[2] = color_map->Colors[color_index].Blue;
                pixel[3] = (color_index == graphic_block.TransparentColor) ? 0x00 : 0xFF;
            }
        }
    }

    *pixels = ctx->FrameBuffer;
    *delay_in_ms = graphic_block.DelayTime * 10;

    ctx->CurrentFrame = (ctx->CurrentFrame + 1) % ctx->Gif->ImageCount;

    return true;
}

void ImMedia_DecoderGiflib_Install()
{
    ImMedia::InstallImageDecoder("gif", {
        CreateContextFromFile,
        CreateContextFromData,
        DeleteContext,
        GetInfo,
        BeginReadFrame,
        nullptr
    });
}
