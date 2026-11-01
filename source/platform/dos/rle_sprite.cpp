#include "rle_sprite.h"

#include <string.h>

// C version of the RLE sprite drawing routine in rle_sprite.asm, see there for the data format.
void drawRleSprite(char* spriteData, char* dst, uint16_t lineWidth)
{
    const uint8_t* data = (const uint8_t*)spriteData;
    uint8_t* out = (uint8_t*)dst;
    uint8_t* lineStart = out; // start of the current line

    for (;;)
    {
        uint8_t rleType = *data & 0xC0;
        uint8_t length = *data & 0x3F;
        data++;

        switch (rleType)
        {
            case RLE_TYPE_CONSECUTIVE:
                memcpy(out, data, length);
                out += length;
                data += length;
                break;

            case RLE_TYPE_REPEAT:
                memset(out, *data, length);
                out += length;
                data++;
                break;

            case RLE_TYPE_SKIP:
                if (length == 0)
                {
                    // skip to the start of the next line
                    lineStart += lineWidth;
                    out = lineStart;
                }
                else
                {
                    out += length;
                }
                break;

            case RLE_TYPE_END:
                return;
        }
    }
}
