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

#include "immedia_image.h"

struct DecoderContext
{
    int      Width;
    int      Height;
    int      Channles;

    uint8_t* Data;
    int      DataSize;

    uint8_t* Pixels;
};

static void* CreateContextFromData(const uint8_t* data, size_t data_size)
{
    if (data_size > INT_MAX) 
        return nullptr;

    int width, height, channels;
    if (!stbi_info_from_memory(data, data_size, &width, &height, &channels))
        return nullptr;

    if (channels != STBI_rgb && channels != STBI_rgb_alpha)
        return nullptr;

    DecoderContext* ctx = new DecoderContext();
    ctx->Width    = width;
    ctx->Height   = height;
    ctx->Channles = channels;
    ctx->Data     = new uint8_t[data_size];
    ctx->DataSize = data_size;
    ctx->Pixels   = nullptr;
    memcpy(ctx->Data, data, data_size);
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
        if (ctx->Channles == STBI_rgb)
            *format = ImMedia::PixelFormat::RGB888;
        else if (ctx->Channles == STBI_rgb_alpha)
            *format = ImMedia::PixelFormat::RGBA8888;
        else
        { IM_ASSERT(false); }
    }
}

static bool BeginReadFrame(void* context, uint8_t** pixels, int* delay_in_ms)
{
    DecoderContext* ctx = reinterpret_cast<DecoderContext*>(context);
    ctx->Pixels = stbi_load_from_memory(ctx->Data, ctx->DataSize, &ctx->Width, &ctx->Height, nullptr, STBI_default);
    if (pixels)
    {
        *pixels = ctx->Pixels;
        *delay_in_ms = 0;
        return true;
    }
    return false;
}

static void EndReadFrame(void* context)
{
    DecoderContext* ctx = reinterpret_cast<DecoderContext*>(context);
    stbi_image_free(ctx->Pixels);
    ctx->Pixels = nullptr;
}

void ImMedia_DecoderSTB_Install(DecoderSTBFormat format)
{
    const static ImMedia::ImageDecoder decoder = {
        nullptr,
        CreateContextFromData,
        DeleteContext,
        GetInfo,
        BeginReadFrame,
        EndReadFrame
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
