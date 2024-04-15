#define IMGUI_DEFINE_MATH_OPERATORS
#include "immedia_image.h"

#include <stdio.h>

#include "imgui_internal.h"

namespace ImMedia {

static const char* GetFileExtension(const char* filename);

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

Image::Image(int width, int height, PixelFormat format, const uint8_t* pixels) noexcept
{
    Width = width;
    Height = height;
    const ImageRenderer* renderer = GetImageRenderer();
    RendererContext = renderer->CreateContext(width, height, format, false);
    renderer->WriteFrame(RendererContext, pixels);
}

Image::Image(void* decoder_context, const ImageDecoder* decoder) noexcept
{
    Load(decoder_context, decoder);
}

Image::~Image()
{
    if (DecoderContext)
        Decoder->DeleteContext(DecoderContext);
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

ImTextureID Image::GetTexture() const
{
    return GetImageRenderer()->GetTexture(RendererContext);
}

void Image::Play() const
{
    if (!Decoder || !DecoderContext)
        return;

    const size_t curremt_time = ImGui::GetCurrentContext()->Time * 1000;
    if (curremt_time < NextFrameTime)
        return;
   
    uint8_t* pixels;
    int      delay;
    if (!Decoder->ReadFrame(DecoderContext, &pixels, &delay))
        return;

    Image* p = const_cast<Image*>(this);
    GetImageRenderer()->WriteFrame(RendererContext, pixels);
    p->NextFrameTime = curremt_time + delay;
    if (delay == 0)
    {
        p->NextFrameTime = SIZE_MAX;
        Decoder->DeleteContext(DecoderContext);
        p->DecoderContext = nullptr;
    }
}

void Image::Show(const ImVec2& size, ImageFillMode fill_mode, const ImVec2& uv0, const ImVec2& uv1, const ImVec4& tint_col, const ImVec4& border_col) const
{
    if (!RendererContext)
        return;

    Play();

    if (fill_mode == ImageFillMode::Stretch)
        ImGui::Image(GetTexture(), size, uv0, uv1, tint_col, border_col);
    else if (fill_mode == ImMedia::ImageFillMode::Fill)
    {
        int p0_x = uv0.x * Width;
        int p0_y = uv0.y * Height;
        int p1_x = uv1.x * Width;
        int p1_y = uv1.y * Height;

        int w = p1_x - p0_x;
        int h = p1_y - p0_y;

        // I couldn't have found a better name.
        double r = fmax(size.x / w, size.y / h);
        int s_w = size.x / r;
        int s_h = size.y / r;

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

        ImVec2 p0 = ImVec2(p3_x, p3_y) / ImVec2(Width, Height);
        ImVec2 p1 = ImVec2(p4_x, p4_y) / ImVec2(Width, Height);

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

        int w = Width * (uv1.x - uv0.x);
        int h = Height* (uv1.y - uv0.y);
        double r = fmin(size.x / w, size.y / h);
        w *= r;
        h *= r;
        ImVec2 offset((size.x - w) / 2, (size.y - h) / 2);

        if (border_size > 0.0f)
            window->DrawList->AddRect(bb.Min + offset, bb.Max - offset, ImGui::GetColorU32(border_col), 0.0f, ImDrawFlags_None, border_size);
        window->DrawList->AddImage(GetTexture(), bb.Min + padding + offset, bb.Max - padding - offset, uv0, uv1, ImGui::GetColorU32(tint_col));
    }
}

void Image::Load(const char* filename, const ImageDecoder* decoder)
{
    if (!filename || !decoder)
        return;
    if (decoder->CreateContextFromFile)
        Load(decoder->CreateContextFromFile(filename), decoder);
    else
    {
        FILE* f = fopen(filename, "rb");
        if (f == nullptr)
            return;
        fseek(f, 0, SEEK_END);
        size_t file_size = ftell(f);
        fseek(f, 0, SEEK_SET);
        uint8_t* data = new uint8_t[file_size];
        fread(data, 1, file_size, f);
        fclose(f);
        Load(data, file_size, decoder);
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
    if (!decoder_context || !decoder)
        return;
    PixelFormat format;
    int         framt_count;
    decoder->GetInfo(decoder_context, &Width, &Height, &format, &framt_count);
    Decoder          = decoder;
    DecoderContext  = decoder_context;
    RendererContext = GetImageRenderer()->CreateContext(Width, Height, format, framt_count != 0);
    Play();
}


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

}
