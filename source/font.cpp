#include "font.h"

#include "exception.h"

#include <stdio.h>
#include <string.h>

#define safeRead(dest, elemSize, count, stream) \
    { \
        size_t readElements = fread(dest, elemSize, count, stream); \
        if (readElements != count) \
        { \
            throw Exception("Invalid number of bytes read"); \
        } \
    }

Font::Font(const char* fontFilename) :
    m_characters(NULL),
    m_numCharacters(0),
    m_imageWidth(0),
    m_imageHeight(0),
    m_imageData(NULL)
{
    FILE* fp = fopen(fontFilename, "rb");
    if (!fp)
    {
        throw Exception("Cannot open font file: ", fontFilename);
    }

    try
    {
        uint8_t signature[6];
        safeRead(signature, 1, 6, fp);
        if (memcmp(signature, "stf252", 6) != 0)
        {
            throw Exception("Invalid font file header");
        }

        uint16_t formatVersion;
        safeRead(&formatVersion, 1, sizeof(uint16_t), fp);
        if (formatVersion != 1)
        {
            throw Exception("Invalid font file version");
        }

        safeRead(&m_imageWidth, 1, sizeof(uint16_t), fp);
        safeRead(&m_imageHeight, 1, sizeof(uint16_t), fp);

        uint32_t imageDataSize = m_imageWidth * m_imageHeight;

        m_imageData = new uint8_t[imageDataSize];
        safeRead(m_imageData, 1, imageDataSize, fp);

        safeRead(&m_numCharacters, 1, sizeof(uint16_t), fp);
        m_characters = new Character[m_numCharacters];

        for (int i = 0; i < m_numCharacters; ++i)
        {
            Character& character = m_characters[i];
            safeRead(&character.character, 1, sizeof(uint8_t), fp);
            safeRead(&character.bearingLeft, 1, sizeof(int16_t), fp);
            safeRead(&character.bearingTop, 1, sizeof(int16_t), fp);
            safeRead(&character.horizontalAdvance, 1, sizeof(int16_t), fp);
            safeRead(&character.verticalAdvance, 1, sizeof(int16_t), fp);
            safeRead(&character.leftOffset, 1, sizeof(int16_t), fp);
            safeRead(&character.topOffset, 1, sizeof(int16_t), fp);
            safeRead(&character.width, 1, sizeof(int16_t), fp);
            safeRead(&character.height, 1, sizeof(int16_t), fp);
        }

        // loop over pixels and change everything != 0 to 0x0f
        for (int i = 0; i < imageDataSize; ++i)
        {
            if (m_imageData[i] != 0)
            {
                m_imageData[i] =0x0f;
            }
        }

        fclose(fp);
    }
    catch(...)
    {
        fclose(fp);
        throw;
    }
}

Font::~Font()
{
    delete[] m_characters;
    delete[] m_imageData;
}
