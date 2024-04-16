// ImageDecoder using stb_image.
// stb homepage: https://github.com/nothings/stb

#ifndef IMMEDIA_DECODER_STB_H
#define IMMEDIA_DECODER_STB_H

enum class DecoderSTBFormat
{
    BMP = 0x01,
    JPG = 0x02,
    PIC = 0x04,
    PNG = 0x08,
    PNM = 0x10,
    PSD = 0x20,
    TGA = 0x40,
    ALL = 0x8F
};

void ImMedia_DecoderSTB_Install(DecoderSTBFormat format);

#endif // IMMEDIA_DECODER_STB_H
