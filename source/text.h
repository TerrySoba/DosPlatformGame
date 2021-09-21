#ifndef _INCLUDE_TEXT_H
#define _INCLUDE_TEXT_H

#include "drawable.h"
#include "tiny_string.h"
#include "vector.h"

class Text : public Drawable
{
public:
    Text(const char* str, int lineWidth = 0);

    virtual int16_t width() const;
    virtual int16_t height() const;
    virtual void draw(char* target, int16_t targetWidth, int16_t targetHeight, int16_t x, int16_t y) const;

private:
    tnd::vector<TinyString> m_text;
    int16_t m_width;
    int16_t m_height;
};


#endif