# 安装自定义解码器

## 仓库自带的解码器

ImMedia 仓库目前附带以下解码器:

|                                                                 | 支持格式                          |
| --------------------------------------------------------------- | --------------------------------- |
| [GIFLIB](https://giflib.sourceforge.net/)                       | gif                               |
| [libjpeg-turbo](https://github.com/libjpeg-turbo/libjpeg-turbo) | jpeg                              |
| [libpng](http://www.libpng.org/pub/png/libpng.html)             | png                               |
| [libwebp](https://github.com/webmproject/libwebp)               | webp                              |
| [qoi](https://github.com/phoboslab/qoi)                         | qoi                               |
| [stb](https://github.com/nothings/stb)                          | bmp, jpg, pic, png, pnm, psd, tga |

## 安装你自己的解码器

要安装自定义解码器，你需要设置以下结构体

```cpp
struct ImageDecoder
{
    void* (*CreateContextFromFile)(void* f, size_t file_size);
    void* (*CreateContextFromData)(const uint8_t* data, size_t data_size);
    void  (*DeleteContext)(void* context);

    void  (*GetInfo)(void* context, int* width, int* height, PixelFormat* format, int* frame_count);

    bool  (*BeginReadFrame)(void* context, uint8_t** pixels, int* delay);
    void  (*EndReadFrame)(void* context);
};
```

`CreateContextFromFile`
- 如果无法解析图片，请返回 `nullptr`
- 如果成功，由解码器负责调用 fclose
- 你可以将其设置为 nullptr，ImMedia 会切换到 `CreateContextFromData`

`CreateContextFromData`
- 如果无法解析图片，请返回 `nullptr`

`GetInfo`
- 如果图片不含动画，请将 `frame_count` 设置为 `0`

`BeginReadFrame`
- 如果成功，请返回 true ，在这之后会调用 BeginReadFrame
- 如果图片不包含动画，或动画已经播放完成，请将 `delay` 设置为 `0`

`BeginReadFrame`
- 可以设置为空
- 在 BeginReadFrame 返回 true 后会被调用

---

设置完成后，使用 `ImMedia::InstallImageDecoder` 安装解码器

```cpp
ImMedia::InstallImageDecoder("format", your_decoder);
```

