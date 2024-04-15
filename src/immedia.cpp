#include "immedia.h"

#include <ctype.h>

namespace ImMedia {

char* ToLower(const char* s);
bool  CompareFormat(const char* format_in_lowercase, const char* s);

struct ImageDecoderInfo
{
    char*        Format;
    ImageDecoder Decoder;
};

struct ImMediaContext
{
    ImVector<ImageDecoderInfo> ImageDecoders;
    ImageRenderer              ImageRenderer;
    bool                       InstallImageRenderer;

    ImMediaContext() :
        ImageRenderer({ nullptr, nullptr,nullptr,nullptr }),
        InstallImageRenderer(false)
    {

    }
};

static ImMediaContext* g_context = nullptr;

void CreateContext()
{
    if (g_context != nullptr)
        return;
    g_context = new ImMediaContext();
}

void DestoryContext()
{
    assert(g_context);
    for (size_t i = 0; i < g_context->ImageDecoders.size(); ++i)
        delete(g_context->ImageDecoders[i].Format);
    delete g_context;
}

void InstallImageDecoder(const char* format, const ImageDecoder& decoder)
{
    assert(g_context);
    assert(decoder.CreateContextFromData);
    assert(decoder.DeleteContext);
    assert(decoder.GetInfo);
    assert(decoder.ReadFrame);

    if (format == nullptr || format[0] == '\0')
        return;

    for (size_t i = 0; i < g_context->ImageDecoders.size(); ++i)
    {
        ImageDecoderInfo& info = g_context->ImageDecoders[i];
        if (CompareFormat(info.Format, format))
        {
            info.Decoder = decoder;
            return;
        }
    }
    g_context->ImageDecoders.push_back({ ToLower(format), decoder});
}

const ImageDecoder* GetImageDecoder(const char* format)
{
    assert(g_context);

    if (format == nullptr || format[0] == '\0')
        return nullptr;

    for (size_t i = 0; i < g_context->ImageDecoders.size(); ++i)
    {
        ImageDecoderInfo& info = g_context->ImageDecoders[i];
        if (CompareFormat(info.Format, format))
            return &info.Decoder;
    }
    return nullptr;
}

void InstallImageRenderer(const ImageRenderer& renderer)
{
    assert(g_context);
    assert(g_context->InstallImageRenderer == false);
    assert(renderer.CreateContext);
    assert(renderer.DeleteContext);
    assert(renderer.GetTexture);
    assert(renderer.WriteFrame);
    g_context->ImageRenderer = renderer;
    g_context->InstallImageRenderer = true;
}

const ImageRenderer* GetImageRenderer()
{ 
    assert(g_context);
    assert(g_context->InstallImageRenderer);
    return &g_context->ImageRenderer;
}

char* ToLower(const char* s)
{
    if (s == nullptr)
        return nullptr;

    int len = strlen(s);
    char* ss = new char[len + 1];
    for (size_t i = 0; i < len; i++)
        ss[i] = tolower(s[i]);
    ss[len] = '\0';
    return ss;
}

bool CompareFormat(const char* format_in_lowercase, const char* s)
{
    int len = strlen(format_in_lowercase);
    if (len != strlen(s))
        return false;

    for (size_t i = 0; i < len; i++)
    {
        if (format_in_lowercase[i] != tolower(s[i]))
            return false;
    }
    return true;
}

}