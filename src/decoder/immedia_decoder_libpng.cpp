#include "immedia_decoder_libpng.h"

#include "png.h"

#include "immedia_image.h"

#define PNG_HEADER_SIZE 8

struct LibpngDecoderContext
{
    png_struct* PNG;
    png_info*   Info;

    uint8_t*    Pixels;
    uint8_t*    Buffer;
};

static void PNGClean(png_struct* png, png_info* info, uint8_t* pixels, uint8_t** rows)
{
    png_destroy_read_struct(&png, &info, nullptr);
    if (pixels)
        delete[] pixels;
    if (rows)
        delete[] rows;
}

static void PNGRead(png_struct* png, png_info* info, uint8_t*& pixels, uint8_t**& rows)
{
    png_read_info(png, info);

    png_uint_32 width      = png_get_image_width(png, info);
    png_uint_32 height     = png_get_image_height(png, info);
    png_byte    color_type = png_get_color_type(png, info);

    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png);
    else if (color_type == PNG_COLOR_TYPE_GRAY)
    {
        png_set_gray_to_rgb(png);
        png_set_expand_gray_1_2_4_to_8(png);
    }

    if (png_get_valid(png, info, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png);

    const bool has_alpha = color_type & PNG_COLOR_MASK_ALPHA;

    size_t row_size = (size_t)width * (has_alpha ? 4 : 3);
    size_t image_size = row_size * height;
    pixels = new uint8_t[image_size];
    rows = new uint8_t* [height];
    for (size_t i = 0; i < height; ++i)
        rows[i] = pixels + i * row_size;
    png_read_image(png, rows);
    delete[] rows;
}

static void* CreateContextFromFile(void* fp, size_t data_size)
{
    uint8_t header[8];
    FILE* f = reinterpret_cast<FILE*>(fp);
    fread(header, 1, PNG_HEADER_SIZE, f);
    if (png_sig_cmp(header, 0, PNG_HEADER_SIZE) != 0)
        return nullptr;

    png_struct* png    = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    png_info*   info   = png_create_info_struct(png);
    png_byte*   pixels = nullptr;
    png_byte**  rows   = nullptr;

    if (setjmp(png_jmpbuf(png)))
    {
        PNGClean(png, info, pixels, rows);
        fclose(f);
        return nullptr;
    }

    png_init_io(png, f);
    png_set_sig_bytes(png, PNG_HEADER_SIZE);
    PNGRead(png, info, pixels, rows);
    fclose(f);
    return new LibpngDecoderContext{ png, info, pixels };
}

struct PNGIO
{
    const uint8_t* Data;
    size_t         DataSize;
    size_t         CurrentPos;
};

static void PNGDataReadFunc(png_structp png, png_bytep png_data, png_size_t length)
{
    PNGIO* png_io = reinterpret_cast<PNGIO*>(png_get_io_ptr(png));
    const uint8_t* data = png_io->Data;
    memcpy(png_data, data + png_io->CurrentPos, length);
    png_io->CurrentPos += length;
}

static void* CreateContextFromData(const uint8_t* data, size_t data_size)
{
    if (data_size <= PNG_HEADER_SIZE)
        return nullptr;

    if (png_sig_cmp(data, 0, PNG_HEADER_SIZE) != 0)
        return nullptr;

    png_struct* png    = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    png_info*   info   = png_create_info_struct(png);
    png_byte*   pixels = nullptr;
    png_byte**  rows   = nullptr;

    if (setjmp(png_jmpbuf(png)))
    {
        PNGClean(png, info, pixels, rows);
        return nullptr;
    }

    PNGIO png_io = { data + PNG_HEADER_SIZE, data_size - PNG_HEADER_SIZE, 0 };
    png_set_sig_bytes(png, PNG_HEADER_SIZE);
    png_set_read_fn(png, &png_io, PNGDataReadFunc);
    PNGRead(png, info, pixels, rows);
    return new LibpngDecoderContext{ png, info, pixels };
}

static void DeleteContext(void* context)
{
    LibpngDecoderContext* ctx = reinterpret_cast<LibpngDecoderContext*>(context);
    png_destroy_read_struct(&ctx->PNG, &ctx->Info, nullptr);
    delete[] ctx->Pixels;
    delete ctx;
}

static void GetInfo(void* context, int* width, int* height, ImMedia::PixelFormat* format, int* frame_count)
{
    LibpngDecoderContext* ctx = reinterpret_cast<LibpngDecoderContext*>(context);
    *width = png_get_image_width(ctx->PNG, ctx->Info);
    *height = png_get_image_height(ctx->PNG, ctx->Info);
    png_byte color_type = png_get_color_type(ctx->PNG, ctx->Info);
    if (color_type == PNG_COLOR_TYPE_RGB)
        *format = ImMedia::PixelFormat::RGB888;
    else if (color_type == PNG_COLOR_TYPE_RGBA)
        *format = ImMedia::PixelFormat::RGBA8888;
    else
    { assert(false); }
    *frame_count = 0;
}

static bool BeginReadFrame(void* context, uint8_t** pixels, int* delay_in_ms)
{
    LibpngDecoderContext* ctx = reinterpret_cast<LibpngDecoderContext*>(context);
    *pixels = ctx->Pixels;
    *delay_in_ms = 0;
    return true;
}

void ImMedia_DecoderLibpng_Install()
{
    ImMedia::InstallImageDecoder("png", {
        CreateContextFromFile,
        CreateContextFromData,
        DeleteContext,
        GetInfo,
        BeginReadFrame,
        nullptr
    });
}
