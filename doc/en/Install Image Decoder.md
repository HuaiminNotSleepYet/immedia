# How to: install new image decoder

To install an image decoder for ImMedia, you need to set up this struct:

```cpp
struct ImageDecoder
{
    void* (*CreateContextFromFile)(const char* filename);
    void* (*CreateContextFromData)(const uint8_t* data, size_t data_size);
    void  (*DeleteContext)(void* context);

    void  (*GetInfo)(void* context, int* width, int* height, PixelFormat* format, int* frame_count);

    bool  (*ReadFrame)(void* context, uint8_t** pixels, int* delay);
};
```

For `CreateContextXXX`, return a `nullptr` if the data cannot be parsed.
> You can set `CreateContextFromFile` to nullptr, ImMedia would switch to `CreateContextFromData`.

For `GetInfo`, `frame_count` should be set to `0` if the image doesn't contain animation.

For `ReadFrame`, `delay` should be set to `0` if the image dosen't contain animation or the animation has finished playing.

---

Then, install decoder by `ImMedia::InstallImageDecoder`

```cpp
ImMedia::InstallImageDecoder("format", your_decoder);
```
