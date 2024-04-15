# 安装自定义解码器

要安装自定义解码器，你需要设置以下结构体

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

对于 `CreateContextXXX` 方，如果无法解析图片，请返回 `nullptr`
> 你可以将 `CreateContextFromFile` 设置为 nullptr，ImMedia 会自动使用 `CreateContextFromData`

对于 `GetInfo` 方法，如果图片不含动画，请将 `frame_count` 设置为 `0`

对于 `ReadFrame`，如果图片不包含动画，或动画已经播放完成，请将 `delay` 设置为 `0`

---

设置完成后，使用 `ImMedia::InstallImageDecoder` 方法安装解码器

```cpp
ImMedia::InstallImageDecoder("format", your_decoder);
```

