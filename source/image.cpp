#include "image.h"

#include "exception.h"

#include <stdio.h>
#include <string.h>


Image::Image(const Image& image)
{
    int s = image.width() * image.height() / 2;
    m_data = new char[s];
    memcpy(m_data, image.data(), s);
}

Image& Image::operator=(const Image& other)
{
    int s = other.width() * other.height();
    delete m_data;
    m_data = new char[s];
    memcpy(m_data, other.data(), s);
    return *this;    
}

Image::Image(const char* filename)
{
    FILE* fp = fopen(filename, "rb");
    if (!fp)
    {
        THROW_EXCEPTION("Could not open file:", filename);
    }

    fread(&m_width, 2, 1, fp);
    fread(&m_height, 2, 1, fp);

    m_data = new char[m_width * m_height];

    fread(m_data, 1, m_width * m_height, fp);

    fclose(fp);
}

Image::Image(int width, int height, uint8_t value) :
    m_width(width),
    m_height(height)
{
    int s = m_width * m_height;
    m_data = new char[s];
    memset(m_data, value, s);
}

Image::~Image()
{
    delete[] m_data;
}
