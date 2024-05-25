# ImMedia

- [English](./README.md)
- [中文](./doc/zh_cn/README.md)

---

Image extensions for [imgui](https://github.com/ocornut/imgui).

![](./doc/en/img/screenshot.png)

---

- [Features](#features)
  - [Image](#image)
  - [Vector graphics](#vector-graphics)
- [Install](#install)
  - [vcpkg](#vcpkg)

## Features

### Image

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

> About how to install new decoder, see also [Install Image](./doc/en/Install%20Image%20Decoder.md)

### Vector graphics

```cpp
#include "immedia_vector_graphics.h"

int main(int argc, char** argv)
{
    // Create ImGui context.

    // VectorGraphics does not require to create context.
	ImMedia::VectorGraphics steve({ 8, 8 });
	ImU32 skin_col = IM_COL32(0xEF, 0xAE, 0x73, 0xFF);
	ImU32 hair_col = IM_COL32(0x59, 0x33, 0x14, 0xFF);
	ImU32 eyes_col = IM_COL32(0x6B, 0x4A, 0xA3, 0xFF);
	ImU32 nose_col = IM_COL32(0x94, 0x52, 0x3A, 0xFF);
	ImU32 mouse_col = IM_COL32(0xD7, 0x6F, 0x52, 0xFF);
	ImU32 beard_col = IM_COL32(0x59, 0x33, 0x14, 0xFF);
	steve.AddRectFilled({ 0, 0 }, { 8, 3 }, hair_col);
	steve.AddRectFilled({ 0, 3 }, { 8, 8 }, skin_col);
	steve.AddRectFilled({ 1, 2 }, { 7, 3 }, skin_col);
	steve.AddRectFilled({ 1, 4 }, { 3, 5 }, IM_COL32_WHITE);
	steve.AddRectFilled({ 5, 4 }, { 7, 5 }, IM_COL32_WHITE);
	steve.AddRectFilled({ 2, 4 }, { 3, 5 }, eyes_col);
	steve.AddRectFilled({ 5, 4 }, { 6, 5 }, eyes_col);
	steve.AddRectFilled({ 3, 5 }, { 5, 6 }, nose_col);
	steve.AddRectFilled({ 3, 6 }, { 5, 7 }, mouse_col);
	steve.AddRectFilled({ 2, 6 }, { 3, 7 }, beard_col);
	steve.AddRectFilled({ 5, 6 }, { 6, 7 }, beard_col);
	steve.AddRectFilled({ 2, 7 }, { 6, 8 }, beard_col);
    // ...
    while(...) // Event loop
    {
        // ...
		steve.Show({ 128, 128 });
		ImMedia::VGButton(vg, { 128, 128 });
        // ...
    }   
    
    // Destory ImGui context.

    return 0;
}
```
Result

![](./doc/en/img/vg_steve.png)

## Install

### vcpkg

Download `vcpkg-port.zip` from [release](https://github.com/HuaiminNotSleepYet/immedia/releases) page, unzip and add to vcpkg's overlay ports.
