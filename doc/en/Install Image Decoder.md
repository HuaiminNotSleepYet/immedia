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

For `CreateContextFromFile`
- You can set it to nullptr, immedia would switch to `CreateContextFromData`.
- Return `nullptr` if the data cannot be parsed.

For `CreateContextFromData`
- Return `nullptr` if the data cannot be parsed.

For `GetInfo`
- `frame_count` should be set to `0` if the image doesn't contain animation.

For `BeginReadFrame`
- Return true if sucess, then EndReadFrame would be called.
- `delay` should be set to `0` if the image dosen't contain animation or the animation has finished playing.

For `EndReadFrame`
- You can set it to nullptr.
- It would be called after BeginReadFrame return ture.

---

Then, install decoder by `ImMedia::InstallImageDecoder`

```cpp
ImMedia::InstallImageDecoder("format", your_decoder);
```
