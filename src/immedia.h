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
    void* (*CreateContextFromFile)(const char* filename);
    void* (*CreateContextFromData)(const uint8_t* data, size_t data_size);
    void  (*DeleteContext)(void* context);

    void  (*GetInfo)(void* context, int* width, int* height, PixelFormat* format, int* frame_count);

    bool  (*ReadFrame)(void* context, uint8_t** pixels, int* delay);
};

struct ImageRenderer
{
    void* (*CreateContext)(int width, int height, PixelFormat format, bool has_anim);
    void  (*DeleteContext)(void* context);

    void  (*WriteFrame)(void* context, const uint8_t* pixels);

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
