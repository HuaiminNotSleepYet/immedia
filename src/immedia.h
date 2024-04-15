// immedia
//
// Image loading library for imgui (https://github.com/ocornut/imgui).
//
//
// Note: The lifecycle of ImMedia object must be between ImMedia::CreateContext() and ImMedia::DestoryContext().
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

#ifndef IMMEDIA_H
#define IMMEDIA_H

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
    /// @brief Create context from filename. This func can be set to nullptr, immedia would switch to @ref CreateContextFromData.
    /// @return [nullable] nullptr if can't parsered from data.
    void* (*CreateContextFromFile)(const char* filename);
    
    /// @brief Create context from memory.
    /// @return [nullable] nullptr if can't parsered from data.
    void* (*CreateContextFromData)(const uint8_t* data, size_t data_size);

    /// @brief Delete context.
    /// @param decoder Decoder context.
    void  (*DeleteContext)(void* context);

    /// @brief Get image info from decoder context.
    /// @param context Decoder context.
    /// @param[out] width [nullable] Image width.
    /// @param[out] height [nullable] Image height.
    /// @param[out] format [nullable] Image format, see also @ref PixelFormat.
    /// @param[out] frame_count [nullable] 0 if the image doesn't contain animation.
    void  (*GetInfo)(void* context, int* width, int* height, PixelFormat* format, int* frame_count);

    /// @brief Read frame from decoder context.
    /// @param context Decoder context.
    /// @param[out] pixels [nullable] A pointer to frame pixels, owned by decoder context.
    /// @param[out] delay [nullable] In milliseconds. 0 if the image dosen't contain animation or the animation has finished playing.
    bool  (*ReadFrame)(void* context, uint8_t** pixels, int* delay);
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

}

#endif // IMMEDIA_H
