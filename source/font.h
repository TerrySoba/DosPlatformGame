#ifndef _FONT_H_INCLUDED
#define _FONT_H_INCLUDED

#include "image_base.h"

#include <stdint.h>

struct Character
{
    uint8_t character; // character in cp437 encoding
    int16_t bearingLeft; // left bearing of character
    int16_t bearingTop; // top bearing of character
    int16_t horizontalAdvance; // horizontal advance of character
    int16_t verticalAdvance; // vertical advance of character

    // Position of character graphic in image
    int16_t leftOffset; // left offset of character in image
    int16_t topOffset; // top offset of character in image
    int16_t width; // width of character
    int16_t height; // height of character
};


class Font : public ImageBase
{
public:
    Font(const char* fontFilename);

    const Character* getCharacters() const { return m_characters; }
    int getNumCharacters() const { return m_numCharacters; }
   
    virtual int16_t width() const { return m_imageWidth; }
	virtual int16_t height() const { return m_imageHeight; }
	virtual char* data() const { return (char*)m_imageData; }

    virtual ~Font();

private:
    Character* m_characters;
    int m_numCharacters;
    int m_imageWidth;
    int m_imageHeight;
    uint8_t* m_imageData;
};


#endif