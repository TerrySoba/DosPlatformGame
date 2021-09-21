#ifndef _DRAWABLE_H_INCLUDED
#define _DRAWABLE_H_INCLUDED

#include "image_base.h"

class Drawable
{
public:
    virtual ~Drawable() {}

    virtual int16_t width() const = 0;
    virtual int16_t height() const = 0;
    virtual void draw(char* target, int16_t targetWidth, int16_t targetHeight, int16_t x, int16_t y) const = 0;
};

#endif