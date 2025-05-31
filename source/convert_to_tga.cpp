#include "convert_to_tga.h"

#include <stdio.h>
#include "rgbi_colors.h"

void write16bit(uint16_t data, FILE* fp)
{
    fwrite(&data, 2, 1, fp);
}

void write8bit(uint8_t data, FILE* fp)
{
    fwrite(&data, 1, 1, fp);
}

void convertToTga(char* data, const char* filename)
{
    FILE* fp = fopen(filename, "wb");

    write8bit(0, fp); // id length in bytes
    write8bit(1, fp); // color map type (1 == color map / palette present)
    write8bit(1, fp); // image type (1 == uncompressed color-mapped image)
    write16bit(0, fp); // color map origin
    write16bit(16, fp); // color map length
    write8bit(24, fp); // color map depth (bits per palette entry)
    write16bit(0, fp); // x origin
    write16bit(0, fp); // y origin
    write16bit(320, fp); // image width
    write16bit(200, fp); // image height
    write8bit(8, fp); // bits per pixel
    write8bit((1 << 5), fp); // image descriptor

    // we do not have id data, so do not write any
    fwrite(rgbiColors, 1, 16 * 3, fp);

    // write uncompressed
    fwrite(data, 1, 320 * 200, fp);
    
    fclose(fp);
}