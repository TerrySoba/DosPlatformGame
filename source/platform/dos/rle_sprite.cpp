#include "rle_sprite.h"

#include <string.h>

void drawRleSprite(char* spriteData, char* dst, uint16_t lineWidth)
{
    uint8_t* lastLineStart = (uint8_t*)dst; // store the start of the current line
    for (uint16_t y = 0; y < lineWidth; y++)
    {
        uint8_t rleType = *spriteData & 0xC0;
        uint8_t length = *spriteData & 0x3F;
        spriteData++;

        switch (rleType)
        {
            case RLE_TYPE_CONSECUTIVE:
                memcpy(dst, spriteData, length);
                dst += length;
                spriteData += length;
                break;

            case RLE_TYPE_REPEAT:
                memset(dst, *spriteData, length);
                dst += length;
                spriteData++;
                break;

            case RLE_TYPE_SKIP:
                if (length == 0)
                {
                    dst = (char*)lastLineStart + lineWidth;  // skip to the end of the line
                    lastLineStart = (uint8_t*)dst; // update the lastLineStart to the new line
                }
                else
                {
                    dst += length;
                }
                
                break;

            case RLE_TYPE_END:
                return; // return from the function
        }
    }
}
