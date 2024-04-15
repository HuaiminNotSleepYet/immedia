#include "immedia_decoder_stb.h"

#ifdef _MSC_VER
#pragma warning(disable:6262)
#pragma warning(disable:26451)
#endif

#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_STDIO
#define STBI_NO_GIF
#define STBI_NO_HDR
#include "stb_image.h"

#include "immedia.h"

struct DecoderContext
{
    int                  Width;
    int                  Height;
    ImMedia::PixelFormat Format;
    uint8_t*             Pixels;
};

static void* CreateContextFromData(const uint8_t* data, size_t data_size)
{
    if (data_size > INT_MAX)
        return nullptr;
    int width, height, channels;
    uint8_t* pixels = stbi_load_from_memory(data, data_size, &width, &height, &channels, STBI_default);
    if (!pixels)
        return nullptr;
    ImMedia::PixelFormat format;
    if (channels == STBI_rgb)
        format = ImMedia::PixelFormat::RGB888;
    else if (channels == STBI_rgb_alpha)
        format = ImMedia::PixelFormat::RGBA8888;
    else
    {
        stbi_image_free(pixels);
        return nullptr;
    }
    DecoderContext* ctx = new DecoderContext();
    ctx->Width  = width;
    ctx->Height = height;
    ctx->Format = format;
    ctx->Pixels = pixels;
    return ctx;
}

static void DeleteContext(void* context)
{
    DecoderContext* ctx = reinterpret_cast<DecoderContext*>(context);
    stbi_image_free(ctx->Pixels);
    delete ctx;
}

static void GetInfo(void* context, int* width, int* height, ImMedia::PixelFormat* format, int* frame_count)
{
    DecoderContext* ctx = reinterpret_cast<DecoderContext*>(context);
    if (width)  *width  = ctx->Width;
    if (height) *height = ctx->Height;
    if (frame_count) *frame_count = 0;
    if (format)
    {
        *format = ctx->Format;
    }
}

static bool ReadFrame(void* context, uint8_t** pixels, int* delay)
{
    DecoderContext* ctx = reinterpret_cast<DecoderContext*>(context);
    if (pixels)
        *pixels = ctx->Pixels;
    if (delay)
        *delay = 0;
    return true;
}

void ImMedia_DecoderSTB_Install(DecoderSTBFormat format)
{
    const static ImMedia::ImageDecoder decoder = {
        nullptr,
        CreateContextFromData,
        DeleteContext,
        GetInfo,
        ReadFrame
    };

    if (((int)format & (int)DecoderSTBFormat::BMP) > 0)
        ImMedia::InstallImageDecoder("bmp", decoder);
    if (((int)format & (int)DecoderSTBFormat::JPG) > 0)
    {
        ImMedia::InstallImageDecoder("jpg",  decoder);
        ImMedia::InstallImageDecoder("jpeg", decoder);
    }
    if (((int)format & (int)DecoderSTBFormat::PIC) > 0)
        ImMedia::InstallImageDecoder("pic", decoder);
    if (((int)format & (int)DecoderSTBFormat::PNG) > 0)
        ImMedia::InstallImageDecoder("png", decoder);
    if (((int)format & (int)DecoderSTBFormat::PNM) > 0)
    {
        ImMedia::InstallImageDecoder("pnm", decoder);
        ImMedia::InstallImageDecoder("pgm", decoder);
    }
    if (((int)format & (int)DecoderSTBFormat::PSD) > 0)
        ImMedia::InstallImageDecoder("psd", decoder);
    if (((int)format & (int)DecoderSTBFormat::TGA) > 0)
        ImMedia::InstallImageDecoder("tga", decoder);
}
