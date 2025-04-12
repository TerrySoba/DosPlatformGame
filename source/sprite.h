#ifndef _COMPILED_SPRITE_H_INCLUDED
#define _COMPILED_SPRITE_H_INCLUDED

#include "drawable.h"
#include "pixel_source.h"

#include <stdint.h>


class Sprite : public Drawable
{
public:
    Sprite(const char* filename, int16_t targetWidth);
    Sprite(const PixelSource& image, int16_t targetWidth);
    virtual ~Sprite();

    virtual int16_t width() const;
    virtual int16_t height() const;
    virtual void draw(char* target, int16_t targetWidth, int16_t targetHeight, int16_t targetX, int16_t targetY) const;

private:
    int8_t* m_data;
    uint32_t m_dataSize;
    int16_t m_width;
    int16_t m_height;
    char m_transparentColor;
};

#endif