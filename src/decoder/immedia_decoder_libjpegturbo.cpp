#include "immedia_decoder_libjpegturbo.h"

#include <stdio.h>

#include "turbojpeg.h"

#include "immedia_image.h"

struct DecoderContext
{
    int      Width;
    int      Height;

    tjhandle Handle;
    uint8_t* Buffer;
    size_t   BufferSize;

    uint8_t* Pixels;
};

static DecoderContext* CreateContext(uint8_t* jpeg_buffer, size_t buffer_size)
{
    tjhandle handle = tj3Init(TJINIT_DECOMPRESS);

    if (tj3DecompressHeader(handle, jpeg_buffer, buffer_size) != 0)
    {
        tj3Destroy(handle);
        tj3Free(jpeg_buffer);
        return nullptr;
    }

    return new DecoderContext {
        tj3Get(handle, TJPARAM_JPEGWIDTH),
        tj3Get(handle, TJPARAM_JPEGHEIGHT),
        handle,
        jpeg_buffer,
        buffer_size,
        nullptr
    };
}

static void* CreateContextFromFile(void* f, size_t file_size)
{
    FILE* fp = reinterpret_cast<FILE*>(f);

    uint8_t* buffer = reinterpret_cast<uint8_t*>(tj3Alloc(file_size));
    fread(buffer, 1, file_size, fp);
    fclose(fp);

    return CreateContext(buffer, file_size);
};

static void* CreateContextFromData(const uint8_t* data, size_t data_size)
{
    uint8_t* buffer = reinterpret_cast<uint8_t*>(tj3Alloc(data_size));
    memcpy(buffer, data, data_size);
    return CreateContext(buffer, data_size);
}

static void DeleteContext(void* context)
{
    DecoderContext* ctx = reinterpret_cast<DecoderContext*>(context);
    if (ctx->Handle) tj3Destroy(ctx->Handle);
    if (ctx->Buffer) tj3Free(ctx->Buffer);
    if (ctx->Pixels) delete[] ctx->Pixels;
}

static void GetInfo(void* context, int* width, int* height, ImMedia::PixelFormat* format, int* frame_count)
{
    DecoderContext* ctx = reinterpret_cast<DecoderContext*>(context);
    if (width) *width = ctx->Width;
    if (height) *height = ctx->Height;
    if (format) *format = ImMedia::PixelFormat::RGB888;
    if (frame_count) *frame_count = 0;
}

static bool BeginReadFrame(void* context, uint8_t** pixels, int* delay_in_ms)
{
    const static int decode_pixel_format = TJPF_RGB;

    DecoderContext* ctx = reinterpret_cast<DecoderContext*>(context);

    if (ctx->Handle)
    {
        ctx->Pixels = new uint8_t[ctx->Width * ctx->Height * tjPixelSize[decode_pixel_format]];
        if (tj3Decompress8(ctx->Handle,
                           ctx->Buffer, ctx->BufferSize,
                           ctx->Pixels, ctx->Width * tjPixelSize[decode_pixel_format],
                           decode_pixel_format) == 0)
        {
            tj3Destroy(ctx->Handle);
            tj3Free(ctx->Buffer);
            ctx->Handle = nullptr;
            ctx->Buffer = nullptr;
        }
        else
        {
            tj3Destroy(ctx->Handle);
            tj3Free(ctx->Buffer);
            delete[] ctx->Pixels;
            ctx->Handle = nullptr;
            ctx->Buffer = nullptr;
            ctx->Pixels = nullptr;
            return false;
        }
    }

    *pixels = ctx->Pixels;
    *delay_in_ms = 0;
    return true;
}

void ImMedia_DecoderLibjpegTurbo_Install()
{
    ImMedia::InstallImageDecoder("jpg", {
        CreateContextFromFile,
        CreateContextFromData,
        DeleteContext,
        GetInfo,
        BeginReadFrame,
        nullptr,
    });
    ImMedia::InstallImageDecoder("jpeg", {
        CreateContextFromFile,
        CreateContextFromData,
        DeleteContext,
        GetInfo,
        BeginReadFrame,
        nullptr,
    });
}
