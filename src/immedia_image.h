// immedia_image.h
//
// Image loading library for imgui (https://github.com/ocornut/imgui).
//
//
// Note: The lifecycle of ImMedia::Image must be between ImMedia::CreateContext() and ImMedia::DestoryContext().
//
//   ImMedia::CreateContext();
//   {
//       // ...
//       ImMedia::Image img("filename");
//       // ...
//   } // The life cycle of img ends here.
//   ImMedia::DestoryContext();
//
//
// About the pointer:
//   If there is no [nullable] tag, all pointer parameter and return value should not be nullptr.
// 

#ifndef IMMEDIA_IMAGE_H
#define IMMEDIA_IMAGE_H

#include <stdint.h>

#include "imgui.h"

namespace ImMedia {

enum class PixelFormat
{
    RGB888,
    RGBA8888
};

struct ImageDecoder
{
    /// @brief Create context from filename. This func can be set to null, immedia would switch to @ref CreateContextFromData.
    /// @param f FILE* of a valid file, open with mode "rb" and seek to start. Decoder is responsible to call fclose if success.
    /// @param file_size File size.
    /// @return [nullable] null if can't parsered from file.
    void* (*CreateContextFromFile)(void* f, size_t file_size);

    /// @brief Create context from memory.
    /// @param data Pointer to data.
    /// @param data_size Data size.
    /// @return [nullable] null if can't parsered from data.
    void* (*CreateContextFromData)(const uint8_t* data, size_t data_size);

    /// @brief Delete context.
    /// @param decoder Decoder context.
    void (*DeleteContext)(void* context);

    /// @brief Get image info from decoder context.
    /// @param context Decoder context.
    /// @param[out] width [nullable] Image width.
    /// @param[out] height [nullable] Image height.
    /// @param[out] format [nullable] Image format, see also @ref PixelFormat.
    /// @param[out] frame_count [nullable] 0 if the image doesn't contain animation.
    void (*GetInfo)(void* context, int* width, int* height, PixelFormat* format, int* frame_count);

    /// @brief Start read one frame from decoder context.
    /// @param context Decoder context.
    /// @param[out] pixels A pointer to frame pixels, valid until call @ref EndReadFrame.
    /// @param[out] delay 0 if the image dosen't contain animation or the animation has finished playing.
    /// @return true if success. Call @ref EndReadFrame after if success.
    bool (*BeginReadFrame)(void* context, uint8_t** pixels, int* delay_in_ms);

    /// @brief End read frame, can be set to null. Call it after @ref BeginReadFrame return true.
    /// @param context Decoder context.
    void (*EndReadFrame)(void* context);
};

struct ImageRenderer
{
    /// @brief Create context with the given format.
    /// @param width Image width.
    /// @param height Image height.
    /// @param format Image format, see also @ref PixelFormat.
    /// @param has_anim Whether the image has animation.
    /// @return Renderer context.
    void* (*CreateContext)(int width, int height, PixelFormat format, bool has_anim);

    /// @brief Delete context.
    /// @param decoder Renderer context.
    void  (*DeleteContext)(void* context);

    /// @brief Write pixels to renderer context.
    /// @param context Renderer context.
    /// @param pixels A pointer to pixels, in the same format from @ref CreateContext
    void  (*WriteFrame)(void* context, const uint8_t* pixels);

    /// @brief Get current texture.
    /// @param context Renderer context.
    /// @return ImTextureID of current frame, the value may be different for each call.
    ImTextureID (*GetTexture)(void* context);
};

void CreateContext();
void DestoryContext();

void InstallImageDecoder(const char* format, const ImageDecoder& decoder);
const ImageDecoder*  GetImageDecoder(const char* format);

void InstallImageRenderer(const ImageRenderer& renderer);
const ImageRenderer* GetImageRenderer();

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

    /// @brief Call it to keep animation playing without call \ref Show.
    void Play() const;

    /// @brief Show image, call \ref Play internally.
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
