#include "text.h"

#include "font_data.h"
#include "vector.h"

#include <string.h>

tnd::vector<TinyString> formatString(const char* str, int lineWidth)
{
    tnd::vector<TinyString> text;

    TinyString bufferStr(lineWidth);

    int len = strlen(str);
    int lastSpace = 0;
    int lastBreak = 0;
    char* buffer = bufferStr.data();

    for (int i = 0; i < len; ++i)
    {
        if (str[i] == ' ' || str[i] == '\n') lastSpace = i;
        if (i - lastBreak >= lineWidth || str[i] == '\n')
        {
            if (lastSpace >= lastBreak)
            {
                int l = lastSpace - lastBreak;
                memcpy(buffer, str + lastBreak, l);
                memset(buffer + l, ' ', lineWidth - l);
                lastBreak = lastSpace + 1;
            }
            else
            {
                memcpy(buffer, str + lastBreak, lineWidth);
                lastBreak = i;
            }
            
            text.push_back(bufferStr);
        }
    }

    int l = len - lastBreak;
    if (l > 0)
    {
        memset(buffer, ' ', lineWidth);
        memcpy(buffer, str + lastBreak, l);
        text.push_back(bufferStr);
    }

    return text;
}


Text::Text(const char* str, int lineWidth)
{
    if (lineWidth == 0)
    {
        m_text.push_back(TinyString(str));
        m_width = m_text[0].size() * CHAR_WIDTH;
        m_height = CHAR_HEIGHT;
    }
    else
    {
        m_text = formatString(str, lineWidth);
        m_width = lineWidth * CHAR_WIDTH;
        m_height = m_text.size() * CHAR_HEIGHT;
    }
    
}

int16_t Text::width() const
{
    return m_width;
}

int16_t Text::height() const
{
    return m_height;
}

void Text::draw(char* target, int16_t targetWidth, int16_t targetHeight, int16_t targetX, int16_t targetY) const
{
    for (int line = 0; line < m_text.size(); ++line)
    {
        int len = m_text[line].size();
        const char* data = m_text[line].c_str();
        for (int i = 0; i < len; ++i)
        {
            char* charImg = alphabet[getCharacterIndex(data[i])];
            for (int16_t y = 0; y < CHAR_HEIGHT; ++y)
            {
                memcpy(target + targetWidth * (targetY + y + line * CHAR_HEIGHT) + targetX + i * CHAR_WIDTH, charImg + CHAR_WIDTH * y, CHAR_WIDTH);
            }
        }
    }
}
