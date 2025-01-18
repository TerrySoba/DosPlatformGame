#include "font_writer.h"
#include "blit.h"

#include <string.h>

FontWriter::FontWriter(const Font* font) :
    m_font(font),
    m_text(0),
    m_characterMap(0),
    m_textWidth(0),
    m_textHeight(0),
    m_maxBearingTop(0)
{
    // Build character map
    m_characterMap = new const Character*[256];
    memset(m_characterMap, 0, sizeof(const Character*) * 256);

    const Character* characters = m_font->getCharacters();

    for (int i = 0; i < m_font->getNumCharacters(); ++i)
    {
        m_characterMap[characters[i].character] = &characters[i];
        if (characters[i].bearingTop > m_maxBearingTop)
        {
            m_maxBearingTop = characters[i].bearingTop;
        }
    }
}

void FontWriter::setText(const char* text)
{
    m_text = text;
    calculateBoundingBox();
}


FontWriter::~FontWriter()
{
    delete[] m_characterMap;
}


int16_t FontWriter::width() const
{
    return m_textWidth;
}

int16_t FontWriter::height() const
{
    return m_textHeight;
}

void FontWriter::draw(char* target, int16_t targetWidth, int16_t targetHeight, int16_t x, int16_t y) const
{
    uint16_t xPos = 0;
    char* fontData = m_font->data();
    uint16_t fontWidth = m_font->width();
    uint16_t fontHeight = m_font->height();
    for (uint16_t i = 0; m_text[i] != 0; ++i)
    {
        const Character* character = m_characterMap[m_text[i]];
        if (character)
        {
            blitTransparent(
                reinterpret_cast<const uint8_t*>(fontData), fontWidth, fontHeight,
                Rectangle(character->leftOffset, character->topOffset, character->width, character->height),
                reinterpret_cast<uint8_t*>(target), targetWidth, targetHeight,
                Point(x + xPos + character->bearingLeft, y - character->bearingTop + m_maxBearingTop),
                0);
            xPos += character->horizontalAdvance;
        }
    }
}

void FontWriter::calculateBoundingBox()
{
    m_textWidth = 0;
    m_textHeight = 0;
    
    for (int i = 0; m_text[i] != 0; ++i)
    {
        const Character* character = m_characterMap[m_text[i]];
        if (character)
        {
            m_textWidth += character->horizontalAdvance;
            m_textHeight = character->verticalAdvance;
        }
    }
}
