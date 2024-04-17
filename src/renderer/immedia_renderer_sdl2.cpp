#include "immedia_renderer_sdl2.h"

#include "SDL2/SDL.h"

#include "immedia_image.h"

static SDL_Renderer* GRenderer = nullptr;

static void* CreateContext(int width, int height, ImMedia::PixelFormat format, bool has_anim)
{
    bool has_alpha = false;
    switch(format)
    {
    case ImMedia::PixelFormat::RGB888:   has_alpha = false; break;
    case ImMedia::PixelFormat::RGBA8888: has_alpha = true;  break;
    default: IM_ASSERT(false); break;
    };
    SDL_Texture* texture = SDL_CreateTexture(GRenderer,
                                             has_alpha ? SDL_PIXELFORMAT_ABGR8888 : SDL_PIXELFORMAT_BGR888,
                                             has_anim ? SDL_TEXTUREACCESS_STREAMING : SDL_TEXTUREACCESS_STATIC,
                                             width, height);
    if (has_alpha)
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    return texture;
}

static void DeleteContext(void* context)
{
    SDL_DestroyTexture(reinterpret_cast<SDL_Texture*>(context));
}

static void WriteFrame(void* context, const uint8_t* pixels)
{
    SDL_Texture* texture = reinterpret_cast<SDL_Texture*>(context);

    int width, height, access;
    Uint32 format;
    SDL_QueryTexture(texture, &format, &access, &width, &height);

    if (access == SDL_TEXTUREACCESS_STATIC)
        SDL_UpdateTexture(texture, nullptr, pixels, width * (format == SDL_PIXELFORMAT_ABGR8888 ? 4 : 3));
    else
    {
        void* texture_pixels;
        int   texture_pitch;
        SDL_LockTexture(texture, nullptr, &texture_pixels, &texture_pitch);
        memcpy(texture_pixels, pixels, (size_t)height * texture_pitch);
        SDL_UnlockTexture(texture);
    }
}

static ImTextureID GetTexture(void* context)
{
    return context;
}

void ImMedia_RendererSDL2_Install(SDL_Renderer* renderer)
{
    GRenderer = renderer;
    ImMedia::InstallImageRenderer({
        CreateContext,
        DeleteContext,
        WriteFrame,
        GetTexture
    });
}

void ImMedia_RendererSDL2_SetHint()
{
    SDL_HINT_RENDER_SCALE_QUALITY;
}
