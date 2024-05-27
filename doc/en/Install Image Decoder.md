# How to: install new image decoder

## Decoder in repository

ImMedia repository contains the following decoders:

|                                                                 | support format                    |
| --------------------------------------------------------------- | --------------------------------- |
| [GIFLIB](https://giflib.sourceforge.net/)                       | gif                               |
| [libjpeg-turbo](https://github.com/libjpeg-turbo/libjpeg-turbo) | jpeg                              |
| [libpng](http://www.libpng.org/pub/png/libpng.html)             | png                               |
| [libwebp](https://github.com/webmproject/libwebp)               | webp                              |
| [qoi](https://github.com/phoboslab/qoi)                         | qoi                               |
| [stb](https://github.com/nothings/stb)                          | bmp, jpg, pic, png, pnm, psd, tga |

## Install your owne decoder

To install an image decoder for immedia, you need to set up this struct:

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

For the functionality of each method, refer to [comments](.../../src/immedia_image.h).

---

Then, install decoder by `ImMedia::InstallImageDecoder`

```cpp
ImMedia::InstallImageDecoder("format", your_decoder);
```
