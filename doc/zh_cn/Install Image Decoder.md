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

    bool  (*ReadFrame)(void* context, uint8_t** pixels, int* delay);
    bool  (*ReadNextFrame)(void* context);
};
```

对于每个方法的功能，请参照 [注释](../../src/immedia_image.h)

---

设置完成后，使用 `ImMedia::InstallImageDecoder` 安装解码器

```cpp
ImMedia::InstallImageDecoder("format", your_decoder);
```
