# ImMedia

- [English](../../README.md)
- [中文](./README.md)

## 快速开始

```cpp
#include ...
#include "immeida.h"
#include "immeida_image.h"
#include "immeida_decoder_stb.h"
#include "immeida_renderer_sdl2.h"

int main(int argc, char** argv)
{
    // 创建 ImGui 上下文

    // 创建 ImMedia 上下文，安装解码器和渲染器
    ImMedia::CreateContext();
    ImMedia_DecoderStb_Install(DecoderSTBFormat::PNG);
    ImMedia_RendererSDL2_Install();

    { // 注意: ImMedia 对象的生命周期必须处在 ImMedia::CreateContext() 与 ImMedia::DestoryContext() 之间
        ImMedia::Image image("./img.png");
        while(true) // 事件循环
        {
            // ...
            image.show({ 256, 256 }, ImMedia::ImageFillMode::Fill);
            // ...
        }
    }
    
    ImMedia::DestoryContext(); // 不要忘记销毁上下文

    // 销毁 ImGui 上下文

    return 0;
}
```

使用 `Image::GetTexture()` 获取当前 `ImTextureID`

```cpp
ImMedia::Image image("./img.png");
ImGui::ImageButton(image.GetTexture(), { 128, 128 });
```

## Decoder

ImMedia 仓库目前附带以下解码器:

|                                                   | 支持格式                          |
| ------------------------------------------------- | --------------------------------- |
| [GIFLIB](https://giflib.sourceforge.net/)         | gif                               |
| [libpng](http://www.libpng.org/pub/png/libpng.html) | png                               |
| [libwebp](https://github.com/webmproject/libwebp) | webp                              |
| [qoi](https://github.com/phoboslab/qoi)           | qoi                               |
| [stb](https://github.com/nothings/stb)            | bmp, jpg, pic, png, pnm, psd, tga |

要安装自定义解码器, 请参照 [安装自定义解码器](./Install%20Image%20Decoder.md)
