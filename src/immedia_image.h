#ifndef IMMEDIA_IMAGE_H
#define IMMEDIA_IMAGE_H

#include "immedia.h"

namespace ImMedia {

enum class ImageFillMode
{
    Stretch,
    Center,
    Fill
};

class Image
{
public:
    Image(const char* filename, const char* format = nullptr) noexcept;
    Image(const char* filename, const ImageDecoder* decoder) noexcept;
    Image(const uint8_t* data, size_t data_size, const char* format) noexcept;
    Image(const uint8_t* data, size_t data_size, const ImageDecoder* decoder) noexcept;
    Image(int width, int height, PixelFormat format, const uint8_t* pixels) noexcept;
    Image(void* decoder_context, const ImageDecoder* decoder) noexcept;
    ~Image();

    Image(const Image&) = delete;
    Image& operator=(const Image&) = delete;
    Image(Image&&) = delete;
    Image& operator=(Image&&) = delete;

    int GetWidth()  const;
    int GetHeight() const;

    ImTextureID GetTexture() const;

    void Play() const;

    void Show(const ImVec2& size,
              ImageFillMode fill_mode = ImageFillMode::Stretch,
              const ImVec2& uv0 = ImVec2(0, 0),
              const ImVec2& uv1 = ImVec2(1, 1),
              const ImVec4& tint_col   = ImVec4(1, 1, 1, 1),
              const ImVec4& border_col = ImVec4(0, 0, 0, 0)) const;

private:
    int                 Width  = 0;
    int                 Height = 0;

    size_t              NextFrameTime = 0;

    void*               DecoderContext  = nullptr;
    const ImageDecoder* Decoder         = nullptr;
    void*               RendererContext = nullptr;

    void Load(const char* filename, const ImageDecoder* decoder);
    void Load(const uint8_t* data, size_t data_size, const ImageDecoder* decoder);
    void Load(void* decoder_context, const ImageDecoder* decoder);
};

}

#endif // IMMEDIA_IMAGE_H
