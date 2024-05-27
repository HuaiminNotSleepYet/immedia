#include "immedia_decoder_qoi.h"

#define QOI_IMPLEMENTATION
#define QOI_NO_STDIO
#include "qoi.h"

#include "immedia_image.h"

static void* CreateContextFromFile(void* f, size_t file_size);
static void* CreateContextFromData(const uint8_t* data, size_t data_size);
static void DeleteContext(void* context);

static void GetInfo(void* context, int* width, int* height, ImMedia::PixelFormat* format, int* frame_count);

static bool ReadFrame(void* context, uint8_t** pixels, int* delay_in_ms);

void ImMedia_DecoderQOI_Install()
{
    ImMedia::InstallImageDecoder("qoi",{
        nullptr,
        CreateContextFromData,
        DeleteContext,
        GetInfo,
        ReadFrame,
        nullptr
    });
}



struct Context
{
    qoi_desc Desc;
    uint8_t* Pixels;
};

static void* CreateContextFromData(const uint8_t* data, size_t data_size)
{
    if (data_size > INT_MAX)
        return nullptr;
    qoi_desc desc;
    uint8_t* pixels = (uint8_t*)qoi_decode(data, static_cast<int>(data_size), &desc, 0);
    return pixels ? new Context{ desc, pixels } : nullptr;
}

static void DeleteContext(void* context)
{
    Context* ctx = reinterpret_cast<Context*>(context);
    delete ctx->Pixels;
    delete ctx;
}

static void GetInfo(void* context, int* width, int* height, ImMedia::PixelFormat* format, int* frame_count)
{
    const qoi_desc& desc = reinterpret_cast<Context*>(context)->Desc;
    if (width)
        *width = desc.width;
    if (height)
        *height = desc.height;
    if (format)
        *format = desc.channels == 3 ? ImMedia::PixelFormat::RGB888 : ImMedia::PixelFormat::RGBA8888;
    if (frame_count)
        *frame_count = 0;
}

static bool ReadFrame(void* context, uint8_t** pixels, int* delay_in_ms)
{
    Context* ctx = reinterpret_cast<Context*>(context);
    *pixels = ctx->Pixels;
    *delay_in_ms = 0;
    return true;
}
