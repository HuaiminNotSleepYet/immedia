#ifdef _MSC_VER
#pragma warning (disable: 4996) // 'This function or variable may be unsafe'.
#endif


#define IMGUI_DEFINE_MATH_OPERATORS
#include "immedia_image.h"

#include <ctype.h>
#include <stdio.h>

#include "imgui_internal.h"

namespace ImMedia {

static bool CompareFormat(const char* format_in_lowercase, const char* s);
static const char* GetFileExtension(const char* filename);



#ifndef IMMEDIA_NO_IMAGE_DECODER

struct ImageDecoderInfo
{
    const char*         Format;
    const ImageDecoder* Decoder;
};

#endif // !IMMEDIA_NO_IMAGE_DECODER


struct ImMediaContext
{
#ifndef IMMEDIA_NO_IMAGE_DECODER
    ImVector<ImageDecoderInfo> ImageDecoders;
#endif

    ImageRenderer* PImageRenderer = nullptr;
    ImageRenderer  ImageRenderer  = {};

    Image* EmptyImage = nullptr;
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

#ifndef IMMEDIA_NO_IMAGE_DECODER
    for (int i = 0; i < g_context->ImageDecoders.size(); ++i)
        delete g_context->ImageDecoders[i].Decoder;
#endif

    if (g_context->EmptyImage)
        delete g_context->EmptyImage;

    delete g_context;
}

#ifndef IMMEDIA_NO_IMAGE_DECODER

void InstallImageDecoder(const char* format, const ImageDecoder& decoder)
{
    assert(g_context);
    assert(decoder.CreateContextFromData);
    assert(decoder.DeleteContext);
    assert(decoder.GetInfo);
    assert(decoder.BeginReadFrame);

    if (format == nullptr || format[0] == '\0')
        return;

    for (int i = 0; i < g_context->ImageDecoders.size(); ++i)
    {
        ImageDecoderInfo& info = g_context->ImageDecoders[i];
        if (CompareFormat(info.Format, format))
        {
            delete info.Decoder;
            info.Decoder = new ImageDecoder(decoder);
            return;
        }
    }
    g_context->ImageDecoders.push_back({ format, new ImageDecoder(decoder) });
}

const ImageDecoder* GetImageDecoder(const char* format)
{
    assert(g_context);

    if (format == nullptr || format[0] == '\0')
        return nullptr;

    for (int i = 0; i < g_context->ImageDecoders.size(); ++i)
    {
        ImageDecoderInfo& info = g_context->ImageDecoders[i];
        if (CompareFormat(info.Format, format))
            return info.Decoder;
    }

    return nullptr;
}

#endif // !IMMEDIA_NO_IMAGE_DECODER

void InstallImageRenderer(const ImageRenderer& renderer)
{
    assert(g_context);
    assert(!g_context->PImageRenderer);
    assert(renderer.CreateContext);
    assert(renderer.DeleteContext);
    assert(renderer.GetTexture);
    assert(renderer.WriteFrame);

    g_context->ImageRenderer = renderer;
    g_context->PImageRenderer = &g_context->ImageRenderer;
    uint8_t pixels[] = { 0x00, 0x00, 0x00, 0x00 };
    g_context->EmptyImage = new Image(1, 1, PixelFormat::RGBA8888, pixels);
}

const ImageRenderer* GetImageRenderer()
{ 
    assert(g_context);
    return g_context->PImageRenderer;
}

#ifndef IMMEDIA_NO_IMAGE_DECODER



Image::Image(const char* filename, const char* format) noexcept
{
    Load(filename, GetImageDecoder(format == nullptr ? GetFileExtension(filename) : format));
}

Image::Image(const char* filename, const ImageDecoder* decoder) noexcept
{
    Load(filename, decoder);
}

Image::Image(const uint8_t* data, size_t data_size, const char* format) noexcept
{
    Load(data, data_size, GetImageDecoder(format));
}

Image::Image(const uint8_t* data, size_t data_size, const ImageDecoder* decoder) noexcept
{
    Load(data, data_size, decoder);
}

Image::Image(void* decoder_context, const ImageDecoder* decoder) noexcept
{
    Load(decoder_context, decoder);
}

#endif // !IMMEDIA_NO_IMAGE_DECODER

Image::Image(int width, int height, PixelFormat format, const uint8_t* pixels) noexcept
{
    Width = width;
    Height = height;
    const ImageRenderer* renderer = GetImageRenderer();
    RendererContext = renderer->CreateContext(width, height, format, false);
    renderer->WriteFrame(RendererContext, pixels);
}

Image::~Image()
{
#ifndef IMMEDIA_NO_IMAGE_DECODER
    if (DecoderContext)
        Decoder->DeleteContext(DecoderContext);
#endif
    if (RendererContext)
        GetImageRenderer()->DeleteContext(RendererContext);
}

int Image::GetWidth() const
{
    return Width;
}

int Image::GetHeight() const
{
    return Height;
}

ImVec2 Image::GetSize() const
{
    return ImVec2((float)Width, (float)Height);
}

bool Image::HasAnimation() const
{
#ifdef IMMEDIA_NO_IMAGE_DECODER
    return false;
#else
    return HasAnim;
#endif
}

ImTextureID Image::GetTexture() const
{
    if (!RendererContext)
        return g_context->EmptyImage->GetTexture();
    Play();
    return GetImageRenderer()->GetTexture(RendererContext);
}

void Image::Play() const
{
#ifndef IMMEDIA_NO_IMAGE_DECODER

    if (!Decoder || !DecoderContext)
        return;

    const size_t curremt_time = static_cast<size_t>(ImGui::GetCurrentContext()->Time * 1000);
    if (curremt_time < NextFrameTime)
        return;

    Image* p = const_cast<Image*>(this);
    uint8_t* pixels;
    int      delay;
    if (Decoder->BeginReadFrame(DecoderContext, &pixels, &delay))
    {
        GetImageRenderer()->WriteFrame(RendererContext, pixels);
        if (Decoder->EndReadFrame)
            Decoder->EndReadFrame(DecoderContext);
        p->NextFrameTime = curremt_time + delay;
        if (delay == 0)
        {
            p->NextFrameTime = SIZE_MAX;
            Decoder->DeleteContext(DecoderContext);
            p->Decoder = nullptr;
            p->DecoderContext = nullptr;
        }
    }
    else
    {
        Decoder->DeleteContext(DecoderContext);
        p->Decoder = nullptr;
        p->DecoderContext = nullptr;
    }

#endif // !IMMEDIA_NO_IMAGE_DECODER
}

void Image::Show(const ImVec2& size, ImageFillMode fill_mode, const ImVec2& uv0, const ImVec2& uv1, const ImVec4& tint_col, const ImVec4& border_col) const
{
    if (!RendererContext)
    {
        ImGui::Dummy(size);
        return;
    }

    Play();

    if (fill_mode == ImageFillMode::Stretch)
        ImGui::Image(GetTexture(), size, uv0, uv1, tint_col, border_col);
    else if (fill_mode == ImMedia::ImageFillMode::Fill)
    {
        // We use int to avoid floating-point rounding errors.
        int p0_x = (int)(uv0.x * Width);
        int p0_y = (int)(uv0.y * Height);
        int p1_x = (int)(uv1.x * Width);
        int p1_y = (int)(uv1.y * Height);

        int w = p1_x - p0_x;
        int h = p1_y - p0_y;

        // I couldn't have found a better name.
        double r = fmax(size.x / w, size.y / h);
        int s_w = (int)(size.x / r);
        int s_h = (int)(size.y / r);

        int p3_x = 0;
        int p3_y = 0;
        int p4_x = w;
        int p4_y = h;

        int x = (w - s_w) / 2;
        int y = (h - s_h) / 2;

        p3_x += x;
        p3_y += y;
        p4_x -= x;
        p4_y -= y;

        p3_x += p0_x;
        p3_y += p0_y;
        p4_x += p0_x;
        p4_y += p0_y;

        ImVec2 p0 = ImVec2((float)p3_x, (float)p3_y) / ImVec2((float)Width, (float)Height);
        ImVec2 p1 = ImVec2((float)p4_x, (float)p4_y) / ImVec2((float)Width, (float)Height);

        ImGui::Image(GetTexture(), size, p0, p1, tint_col, border_col);
    }
    else if (fill_mode == ImageFillMode::Center)
    {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        if (window->SkipItems)
            return;

        const float border_size = (border_col.w > 0.0f) ? 1.0f : 0.0f;
        const ImVec2 padding(border_size, border_size);
        const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + size + padding * 2.0f);
        ImGui::ItemSize(bb);
        if (!ImGui::ItemAdd(bb, 0))
            return;

        int w = (int)(Width  * (uv1.x - uv0.x));
        int h = (int)(Height * (uv1.y - uv0.y));
        double r = fmin(size.x / w, size.y / h);
        w = (int)(r * w);
        h = (int)(r * h);
        ImVec2 offset((size.x - w) / 2, (size.y - h) / 2);

        if (border_size > 0.0f)
            window->DrawList->AddRect(bb.Min + offset, bb.Max - offset, ImGui::GetColorU32(border_col), 0.0f, ImDrawFlags_None, border_size);
        window->DrawList->AddImage(GetTexture(), bb.Min + padding + offset, bb.Max - padding - offset, uv0, uv1, ImGui::GetColorU32(tint_col));
    }
}

#ifndef IMMEDIA_NO_IMAGE_DECODER

void Image::Load(const char* filename, const ImageDecoder* decoder)
{
    if (!filename || !decoder)
        return;

    FILE* f = fopen(filename, "rb");
    if (!f)
        return;

    fseek(f, 0, SEEK_END);
    size_t file_size = ftell(f);
    fseek(f, 0, SEEK_SET);

    if (decoder->CreateContextFromFile)
        Load(decoder->CreateContextFromFile(f, file_size), decoder);
    else
    {
        uint8_t* data = new uint8_t[file_size];
        fread(data, 1, file_size, f);
        fclose(f);
        Load(decoder->CreateContextFromData(data, file_size), decoder);
        delete[] data;
    }
}

void Image::Load(const uint8_t* data, size_t data_size, const ImageDecoder* decoder)
{
    if (!data || !decoder)
        return;
    Load(decoder->CreateContextFromData(data, data_size), decoder);
}

void Image::Load(void* decoder_context, const ImageDecoder* decoder)
{
    assert(GetImageRenderer());

    if (!decoder_context || !decoder)
        return;

    PixelFormat format;
    int         framt_count;
    decoder->GetInfo(decoder_context, &Width, &Height, &format, &framt_count);
    HasAnim = framt_count > 0;
    Decoder          = decoder;
    DecoderContext  = decoder_context;
    RendererContext = GetImageRenderer()->CreateContext(Width, Height, format, HasAnim);

    Play();
}

#endif // !IMMEDIA_NO_IMAGE_DECODER



const char* GetFileExtension(const char* filename)
{
    const char* p0 = filename;
    const char* p1 = nullptr;
    while (*p0 != '\0')
    {
        if (*p0 == '.')
            p1 = p0 + 1;
        ++p0;
    }
    return (p1 == nullptr || *p1 == '\0') ? nullptr : p1;
}

bool CompareFormat(const char* format_in_lowercase, const char* s)
{
    size_t len = strlen(format_in_lowercase);
    if (len != strlen(s))
        return false;

    for (int i = 0; i < len; i++)
    {
        if (format_in_lowercase[i] != tolower(s[i]))
            return false;
    }
    return true;
}

}
