#include "immedia_decoder_libwebp.h"

#include <stdio.h>

#include "webp/demux.h"

#include "immedia.h"

struct WebpDecoderContext
{
    WebPData                WebpData;
    WebPDecoderConfig       DecoderConfig;

    WebPAnimDecoderOptions* AnimDecoderOptions;
    WebPAnimDecoder*        AnimDecoder;
    int                     PreviousTimeStamp;
};

static WebpDecoderContext* CreateContext(const WebPData& webp_data)
{
    if (WebPGetInfo(webp_data.bytes, webp_data.size, nullptr, nullptr) == false)
        return nullptr;

    WebpDecoderContext* ctx = new WebpDecoderContext();
    ctx->WebpData = webp_data;
    WebPInitDecoderConfig(&ctx->DecoderConfig);
    WebPGetFeatures(webp_data.bytes, webp_data.size, &ctx->DecoderConfig.input);

    ctx->DecoderConfig.options.use_threads = true;
    ctx->DecoderConfig.output.colorspace = ctx->DecoderConfig.input.has_alpha ? MODE_RGBA : MODE_RGB;

    if (!ctx->DecoderConfig.input.has_animation)
    {
        ctx->AnimDecoderOptions = nullptr;
        ctx->AnimDecoder = nullptr;
    }
    else
    {
        ctx->AnimDecoderOptions = new WebPAnimDecoderOptions();
        WebPAnimDecoderOptionsInit(ctx->AnimDecoderOptions);
        ctx->AnimDecoder = WebPAnimDecoderNew(&ctx->WebpData, ctx->AnimDecoderOptions);
    }
    ctx->PreviousTimeStamp = 0;

    return ctx;
}

static void* CreateContextFromFile(const char* filename)
{
    FILE* f = fopen(filename, "rb");
    if (!f)
        return nullptr;
    fseek(f, 0, SEEK_END);
    size_t file_size = ftell(f);
    fseek(f, 0, SEEK_SET);
    uint8_t* buffer = (uint8_t*)WebPMalloc(file_size);
    fread(buffer, 1, file_size, f);
    if (!WebPGetInfo(buffer, file_size, nullptr, nullptr))
    {
        delete buffer;
        return nullptr;
    }
    WebPData webp_data = {
        buffer,
        file_size
    };
    return CreateContext(webp_data);
}

static void* CreateContextFromData(const uint8_t* data, size_t data_size)
{
    if (!WebPGetInfo(data, data_size, nullptr, nullptr))
        return nullptr;
    WebPData webp_data = {
        (uint8_t*)WebPMalloc(data_size),
        data_size
    };
    memcpy((void*)webp_data.bytes, data, data_size);
    return CreateContext(webp_data);
}

static void DeleteContext(void* context)
{
    WebpDecoderContext* ctx = reinterpret_cast<WebpDecoderContext*>(context);
    WebPFreeDecBuffer(&ctx->DecoderConfig.output);
    WebPDataClear(&ctx->WebpData);
    if (ctx->AnimDecoderOptions)
    {
        delete ctx->AnimDecoderOptions;
        WebPAnimDecoderDelete(ctx->AnimDecoder);
    }
    delete ctx;
}

static void GetInfo(void* context, int* width, int* height, ImMedia::PixelFormat* format, int* frame_count)
{
    WebpDecoderContext* ctx = reinterpret_cast<WebpDecoderContext*>(context);
    const WebPBitstreamFeatures& feature = ctx->DecoderConfig.input;
    if (width)
        *width = feature.width;
    if (height)
        *height = feature.height;
    if (format)
        *format = feature.has_alpha ? ImMedia::PixelFormat::RGBA8888 : ImMedia::PixelFormat::RGB888;
    if (frame_count)
    {
        if (!feature.has_animation)
            *frame_count = 0;
        else
        {
            WebPAnimInfo anim_info;
            WebPAnimDecoderGetInfo(ctx->AnimDecoder, &anim_info);
            *frame_count = anim_info.frame_count;
        }
    }
}

static bool ReadFrame(void* context, uint8_t** pixels, int* delay)
{
    WebpDecoderContext* ctx = reinterpret_cast<WebpDecoderContext*>(context);
    if (!ctx->DecoderConfig.input.has_animation)
    {
        if (pixels)
        {
            if (ctx->DecoderConfig.output.u.RGBA.rgba == nullptr)
                WebPDecode(ctx->WebpData.bytes, ctx->WebpData.size, &ctx->DecoderConfig);
            *pixels = ctx->DecoderConfig.output.u.RGBA.rgba;
        }
        if (delay)
            *delay = 0;
    }
    else
    {
        if (WebPAnimDecoderHasMoreFrames(ctx->AnimDecoder) == false)
        {
            WebPAnimDecoderReset(ctx->AnimDecoder);
            ctx->PreviousTimeStamp = 0;
        }
        uint8_t* frame;
        int timestamp;
        WebPAnimDecoderGetNext(ctx->AnimDecoder, &frame, &timestamp);
        if (pixels)
            *pixels = frame;
        if (delay)
            *delay = timestamp - ctx->PreviousTimeStamp;
        ctx->PreviousTimeStamp = timestamp;
    }
    return true;
}

void ImMedia_DecoderLibwebp_Install()
{
    ImMedia::InstallImageDecoder("webp", {
        CreateContextFromFile,
        CreateContextFromData,
        DeleteContext,
        GetInfo,
        ReadFrame
    });
}
