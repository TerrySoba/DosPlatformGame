#include "tga_image.h"

#include "exception.h"

#include <stdio.h>
#include <string.h>
#include <malloc.h>

TgaImage::TgaImage(const char* filename) : 
    m_data(NULL),
    m_dataSize(0),
    m_width(0),
    m_height(0)
{
    loadImage(filename);
}

void TgaImage::loadImage(const char* filename)
{
    FILE* fp = fopen(filename, "rb");
    if (!fp)
    {
        throw Exception("Could not open file:", filename);
    }

    uint8_t idLength;
    uint8_t colorMapType;
    uint8_t imageType;
    uint16_t colorMapOrigin;
    uint16_t colorMapLength;
    uint8_t colorMapDepth;
    uint16_t xOrigin;
    uint16_t yOrigin;
    uint16_t width;
    uint16_t height;
    uint8_t bitsPerPixel;
    uint8_t imageDescriptor;
    
    fread(&idLength, 1, 1, fp);
    fread(&colorMapType, 1, 1, fp);
    fread(&imageType, 1, 1, fp);
    fread(&colorMapOrigin, 2, 1, fp);
    fread(&colorMapLength, 2, 1, fp);
    fread(&colorMapDepth, 1, 1, fp);
    fread(&xOrigin, 2, 1, fp);
    fread(&yOrigin, 2, 1, fp);
    fread(&width, 2, 1, fp);
    fread(&height, 2, 1, fp);
    fread(&bitsPerPixel, 1, 1, fp);
    fread(&imageDescriptor, 1, 1, fp);

    // make sure we have palette data
    if (colorMapType != 1 )
    {
        fclose(fp);
        throw Exception("Invalid image colorMapType.");
    }
    
    // make sure this is a Color-mapped Image
    if (imageType != 1 && imageType != 9)
    {
        fclose(fp);
        throw Exception("Invalid image imageType.");
    }

    uint16_t paletteEntrySize = (colorMapDepth == 15)? 16 : colorMapDepth;

    // we ignore id data, so seek over it
    fseek(fp, idLength, SEEK_CUR);

    // seek over color palette for now
    fseek(fp, (uint32_t)(paletteEntrySize / 8) * (uint32_t)colorMapLength, SEEK_CUR);

    if (bitsPerPixel != 8)
    {
        fclose(fp);
        throw Exception("Invalid image bitsPerPixel.");
    }

    size_t size = width * height;

    if (size > m_dataSize)
    {
        free(m_data);
        m_data = (int8_t*)malloc(size);
        m_dataSize = size;
    }
    
    if (imageType == 1) // uncompressed indexed
    {
        fread(m_data, 1, size, fp);
    } else if (imageType == 9) { // compressed indexed
        uint32_t readBytes = 0;
        while (readBytes < size)
        {
            uint8_t chunkHeader = fgetc(fp);
            uint8_t chunkLength = (chunkHeader & 0x7f) + 1;
            bool isRLEChunk = chunkHeader & 0x80;
            if (isRLEChunk)
            {
                uint8_t color = fgetc(fp);
                memset(&m_data[readBytes], color, chunkLength);
            } else {
                fread(&m_data[readBytes], 1, chunkLength, fp);
            }

            readBytes += chunkLength;
        }
    }

    m_width = width;
    m_height = height;

    bool a = imageDescriptor & (1 << 4);
    bool b = imageDescriptor & (1 << 5);

    if (!b) // origin is at bottom
    {
        // flip image vertically
        char* lineStore = (char*)malloc(m_width);

        for (int line = 0; line < height / 2; ++line)
        {
            int8_t* aLine = &m_data[width * line];
            int8_t* bLine = &m_data[width * (height - line - 1)];
            memcpy(lineStore , aLine     , width);
            memcpy(aLine     , bLine     , width);
            memcpy(bLine     , lineStore , width);
        }

        free(lineStore);
    }

    fclose(fp);
}

TgaImage::~TgaImage()
{
    free(m_data);
}

int16_t TgaImage::width() const
{
    return m_width;
}

int16_t TgaImage::height() const
{
    return m_height;
}

char* TgaImage::data() const
{
    return (char*)m_data;
}