#include "sprite.h"
#include "tga_image.h"

#include <string.h>

#include "blit.h"

Sprite::Sprite(const char* filename, int16_t targetWidth)
{
    TgaImage image(filename);
    m_width = image.width();
    m_height = image.height();
    m_dataSize = m_width * m_height;
    m_data = new int8_t[m_dataSize];
    memcpy(m_data, image.data(), m_dataSize);
    m_transparentColor = 0;
}


Sprite::Sprite(const PixelSource& image, int16_t targetWidth)
{
    m_width = image.width();
    m_height = image.height();
    m_dataSize = m_width * m_height;
    m_data = new int8_t[m_dataSize];
    for (int16_t y = 0; y < m_height; ++y)
    {
        for (int16_t x = 0; x < m_width; ++x)
        {
            m_data[y * m_width + x] = image.pixel(x, y);
        }
    }
    m_transparentColor = image.transparentColor();
}

Sprite::~Sprite()
{
    delete[] m_data;
    m_data = nullptr;
    m_dataSize = 0;
    m_width = 0;
    m_height = 0;
}

int16_t Sprite::width() const
{
    return m_width;
}

int16_t Sprite::height() const
{
    return m_height;
}

void Sprite::draw(char* target, int16_t targetWidth, int16_t targetHeight, int16_t targetX, int16_t targetY) const
{
    Rectangle sourceRect(0, 0, m_width, m_height);
    Rectangle targetRect(targetX, targetY, m_width, m_height);
    blitTransparent(
        reinterpret_cast<const uint8_t*>(m_data), m_width, m_height,
        sourceRect,
        reinterpret_cast<uint8_t*>(target), targetWidth, targetHeight,
        Point(targetRect.x, targetRect.y), 0);
}
