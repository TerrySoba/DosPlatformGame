#ifndef _FRAME_IMAGE_H_INCLUDED
#define _FRAME_IMAGE_H_INCLUDED

#include "platform/dos/compiled_sprite.h"
#include "tga_image.h"

#include <stdint.h>

class FrameImage : public PixelSource
{
public:
    FrameImage(TgaImage& img, int16_t x, int16_t y, int16_t w, int16_t h) :
        m_img(img), m_x(x), m_y(y), m_w(w), m_h(h)
    {
    }

    virtual int16_t width() const
    {
        return m_w;
    }

    virtual int16_t height() const
    {
        return m_h;
    }

    virtual char pixel(int16_t x, int16_t y) const
    {
        return m_img.data()[m_img.width() * (m_y + y) + m_x + x];
    }

    virtual char transparentColor() const
    {
        return 0;
    }

private:
    TgaImage& m_img;
    int16_t m_x;
    int16_t m_y;
    int16_t m_w;
    int16_t m_h;
};

#endif