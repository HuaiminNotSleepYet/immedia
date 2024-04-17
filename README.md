# ImMedia

- [English](./README.md)
- [中文](./doc/zh_cn/README.md)

## Quick Start

```cpp
#include ...
#include "immeida_image.h"
#include "immeida_decoder_stb.h"
#include "immeida_renderer_sdl2.h"

int main(int argc, char** argv)
{
    // Create ImGui context.

    // Create ImMedia context, install decoder and renderer.
    ImMedia::CreateContext();
    ImMedia_DecoderStb_Install(DecoderSTBFormat::PNG);
    ImMedia_RendererSDL2_Install();

    { // Note: The lifecycle of ImMedia::Image must be between ImMedia::CreateContext() and ImMedia::DestoryContext().
        ImMedia::Image image("./img.png");
        while(true) // Event loop
        {
            // ...
            image.show({ 256, 256 }, ImMedia::ImageFillMode::Fill);
            // ...
        }   
    }
    
    ImMedia::DestoryContext(); // Don't forget to destory context.

    // Destory ImGui context.

    return 0;
}
```

You can use `Image::GetTexture()` to get current `ImTextureID`.

```cpp
ImMedia::Image image("./img.png");
ImGui::ImageButton(image.GetTexture(), { 128, 128 });
```

## Decoder

ImMedia repository contains the following decoders:

|                                                     | support format                    |
| --------------------------------------------------- | --------------------------------- |
| [GIFLIB](https://giflib.sourceforge.net/)           | gif                               |
| [libpng](http://www.libpng.org/pub/png/libpng.html) | png                               |
| [libwebp](https://github.com/webmproject/libwebp)   | webp                              |
| [qoi](https://github.com/phoboslab/qoi)             | qoi                               |
| [stb](https://github.com/nothings/stb)              | bmp, jpg, pic, png, pnm, psd, tga |

To install new decoder, see also [Install Image](./doc/en/Install%20Image%20Decoder.md)
