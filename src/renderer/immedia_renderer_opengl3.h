// We made it as a single header to avoid using OpenGL loader.
// 
// Place it before other OpenGL header like these:
// 
//     #include "glad/glad.h"
//     #include "GLFW/glfw3.h"
//     #define IMMEDIA_RENDERER_OPENGL3_IMPL
//     #include "immedia_renderer_opengl3.h"
//
// Define the following macros to enable feature:
// 
//     IMMEDIA_RENDERER_OPENGL3_USE_LINEAR_FILTER
//     IMMEDIA_RENDERER_OPENGL3_USE_MIPMAP
//

#ifndef IMMEDIA_RENDERER_OPENGL3_H
#define IMMEDIA_RENDERER_OPENGL3_H

#ifdef _MSC_VER
#pragma warning(disable:4312)
#endif

void ImMedia_RendererOpenGL3_Install();

#ifdef IMMEDIA_RENDERER_OPENGL3_IMPL

#include "immedia.h"

struct OpenGL3RendererContext
{
    int    Width;
    int    Height;
    int    Format;
    GLuint Texture;
};

void* ImMedia_RendererOpenGL3_CreateContext(int width, int height, ImMedia::PixelFormat format, bool has_anim)
{
    OpenGL3RendererContext* ctx = new OpenGL3RendererContext();
    ctx->Width  = width;
    ctx->Height = height;
    ctx->Format = GL_NONE;
    switch (format)
    {
    case ImMedia::PixelFormat::RGB888:   ctx->Format = GL_RGB;  break;
    case ImMedia::PixelFormat::RGBA8888: ctx->Format = GL_RGBA; break;
    }
    glGenTextures(1, &ctx->Texture);
    glBindTexture(GL_TEXTURE_2D, ctx->Texture);

#ifdef IMMEDIA_RENDERER_OPENGL3_USE_LINEAR_FILTER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#else
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
#endif

    return ctx;
}

void ImMedia_RendererOpenGL3_DeleteContext(void* context)
{
    OpenGL3RendererContext* ctx = reinterpret_cast<OpenGL3RendererContext*>(context);
    glDeleteTextures(1, &ctx->Texture);
    delete ctx;
}

void ImMedia_RendererOpenGL3_WriteFrame(void* context, const uint8_t* pixels)
{
    OpenGL3RendererContext* ctx = reinterpret_cast<OpenGL3RendererContext*>(context);
    if (ctx->Format == GL_RGB)
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glBindTexture(GL_TEXTURE_2D, ctx->Texture);
    glTexImage2D(GL_TEXTURE_2D, 0, ctx->Format, ctx->Width, ctx->Height, 0, ctx->Format, GL_UNSIGNED_BYTE, pixels);

#ifdef IMMEDIA_RENDERER_OPENGL3_USE_MIPMAP
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
#else
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
#endif

    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
}

ImTextureID ImMedia_RendererOpenGL3_GetTexture(void* context)
{
    OpenGL3RendererContext* ctx = reinterpret_cast<OpenGL3RendererContext*>(context);
    return reinterpret_cast<ImTextureID>(ctx->Texture);
}

void ImMedia_RendererOpenGL3_Install()
{
    ImMedia::InstallImageRenderer({
        ImMedia_RendererOpenGL3_CreateContext,
        ImMedia_RendererOpenGL3_DeleteContext,
        ImMedia_RendererOpenGL3_WriteFrame,
        ImMedia_RendererOpenGL3_GetTexture
    });
}

#endif // IMMEDIA_RENDERER_OPENGL3_IMPL

#ifdef _MSC_VER
#pragma warning(default:4312)
#endif

#endif // IMMEDIA_RENDERER_OPENGL3_H
