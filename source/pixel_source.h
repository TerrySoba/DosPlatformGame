#ifndef PIXEL_SOURCE_H
#define PIXEL_SOURCE_H

#include <stdint.h>

class PixelSource
{
public:
    virtual int16_t width() const = 0;
    virtual int16_t height() const = 0;
    virtual char pixel(int16_t x, int16_t y) const = 0;
    virtual char transparentColor() const = 0;
};


#endif
