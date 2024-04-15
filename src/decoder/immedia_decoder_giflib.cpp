#include "immedia_decoder_giflib.h"

#include "gif_lib.h"

#include "immedia.h"

struct GiflibDecoderContext
{
    GifFileType* Gif;
    bool         HasAlpha;

    uint8_t*     FrameBuffer;
    int          CurrentFrame;
};

static GiflibDecoderContext* GifContinuRead(GifFileType* gif)
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

static int GifInputFunc(GifFileType* gif, GifByteType* buf, int len)
{
    memcpy(buf, gif->UserData, len);
    gif->UserData = ((uint8_t*)gif->UserData) + len;
    return len;
}

static void* CreateContextFromFile(const char* filename)
{
    return GifContinuRead(DGifOpenFileName(filename, nullptr));
}

static void* CreateContextFromData(const uint8_t* data, size_t data_size)
{
    return GifContinuRead(DGifOpen((void*)data, GifInputFunc, nullptr));
}

static void  DeleteContext(void* context)
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

static bool ReadFrame(void* context, uint8_t** pixels, int* delay)
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

    if (pixels)
        *pixels = ctx->FrameBuffer;
    if (delay)
        *delay = graphic_block.DelayTime * 10;

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
        ReadFrame
    });
}
