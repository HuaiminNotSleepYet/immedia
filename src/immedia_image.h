// Image extension for imgui 1.90.6 (https://github.com/ocornut/imgui).
// 
// About the pointer:
//   All pointer parameter and return value should not be nullptr if there is no [nullable] tag.
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
//  Define IMMEDIA_NO_IMAGE_DECODER macro to disable decoder feature.
//

#ifndef IMMEDIA_IMAGE_H
#define IMMEDIA_IMAGE_H

#include <stdint.h>

#include "imgui.h"

namespace ImMedia {

void CreateContext();
void DestoryContext();



// The structure of pixelformat:
// | 0   -   15 | 16    | 17  -  32 |
// | pixel size | alpha | format id |

#define PIXEL_FORMAT_INFO(ID, HAS_ALPHA, SIZE) \
    ((0xFF & ID       ) << 9)\
  | ((0x01 & HAS_ALPHA) << 8)\
  | ((0xFF & SIZE))

enum class PixelFormat : int
{                            // | ID | alpha | size |
    RGB888   = PIXEL_FORMAT_INFO( 1,   0,      3    ),
    RGBA8888 = PIXEL_FORMAT_INFO( 2,   1,      4    ),
};

#define PIXEL_FORMAT_SIZE(PIXEL_FORMAT)      ((int)PIXEL_FORMAT & 0x0FF)
#define PIXEL_FORMAT_HAS_ALPHA(PIXEL_FORMAT) ((int)PIXEL_FORMAT & 0x100)



#ifndef IMMEDIA_NO_IMAGE_DECODER

struct ImageDecoder
{
    /// @brief Create context from filename.
    ///        It can be set to null, immedia would switch to @ref CreateContextFromData.
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

    /// @brief Read current frame from decoder context.
    /// @param context Decoder context.
    /// @param[out] pixels A pointer to frame pixels, valid until call @ref ReadNext.
    /// @param[out] delay 0 if the image dosen't contain animation or is the last frame.
    /// @return true if success.
    bool (*ReadFrame)(void* context, uint8_t** pixels, int* delay_in_ms);

    /// @brief Read next frame from decoder context.
    ///        It can be set to null if the decoder doesn't support animation.
    /// @param context Decoder context.
    /// @return true if has next frame.
    bool (*ReadNextFrame)(void* context);
};

/// @brief Installs decoder for the specified format.
///        Note: If install for an existing format, the old decoder would be overwritten.
/// @param format Image format string in lowercase, must keep valid before call @ref DestoryContext.
/// @param decoder ImageDecoder struct.
void InstallImageDecoder(const char* format, const ImageDecoder& decoder);

/// @brief Get decoder for the format.
/// @param format Image format string, case insensitive.
/// @return [nullable] null if no corresponding decoder is installed.
const ImageDecoder* GetImageDecoder(const char* format);

#endif // !IMMEDIA_NO_IMAGE_DECODER



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
#ifndef IMMEDIA_NO_IMAGE_DECODER
    Image(const char* filename, const char* format = nullptr) noexcept;
    Image(const char* filename, const ImageDecoder* decoder) noexcept;
    Image(const uint8_t* data, size_t data_size, const char* format) noexcept;
    Image(const uint8_t* data, size_t data_size, const ImageDecoder* decoder) noexcept;
    Image(void* decoder_context, const ImageDecoder* decoder) noexcept;
#endif // !IMMEDIA_NO_IMAGE_DECODER

    Image(int width, int height, PixelFormat format, const uint8_t* pixels) noexcept;

    ~Image();

    Image(const Image&) = delete;
    Image& operator=(const Image&) = delete;
    Image(Image&&) = default;
    Image& operator=(Image&&) = default;

    int GetWidth() const;
    int GetHeight() const;
    ImVec2 GetSize() const;

    bool HasAnimation() const;

    /// @brief Get current ImTextureID.
    /// @return For non-animation image, the ImTextureID keep valid until destroyed.
    ///         For animation image, the ImTextureID may changed when each called.
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
    int    Width   = 0;
    int    Height  = 0;

    void*  RendererContext = nullptr;

#ifndef IMMEDIA_NO_IMAGE_DECODER
    void*               DecoderContext  = nullptr;
    const ImageDecoder* Decoder         = nullptr;
    bool                HasAnim         = false;
    size_t              NextFrameTime   = 0;

    void Load(const char* filename, const ImageDecoder* decoder);
    void Load(const uint8_t* data, size_t data_size, const ImageDecoder* decoder);
    void Load(void* decoder_context, const ImageDecoder* decoder);
#endif // !IMMEDIA_NO_IMAGE_DECODER
};

}

#endif // !IMMEDIA_IMAGE_H
