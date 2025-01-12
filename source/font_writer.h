#ifndef _FONT_WRITER_H_
#define _FONT_WRITER_H_

#include "font.h"
#include "drawable.h"
#include "tiny_string.h"

class FontWriter : public Drawable
{
public:
    FontWriter(const Font* font);
    void setText(const char* text);

    virtual ~FontWriter();

    virtual int16_t width() const;
    virtual int16_t height() const;
    virtual void draw(char* target, int16_t targetWidth, int16_t targetHeight, int16_t x, int16_t y) const;

    

private:

    void calculateBoundingBox();

    // The character map has 256 elements. The index is interpreted as the character code.
    // This way a character can be looked up in O(1) time.
    // Example usage: characterMap['A']
    // If a character is not found in the map, the pointer will be a null pointer.
    const Character** m_characterMap;

    const Font* m_font;
    TinyString m_text;

    uint16_t m_textWidth;
    uint16_t m_textHeight;

    int16_t m_maxBearingTop;
};

#endif