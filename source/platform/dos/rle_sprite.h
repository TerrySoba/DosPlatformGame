#ifndef RLE_SPRITE_H
#define RLE_SPRITE_H

#include <stdint.h>

enum RleType
{
    RLE_TYPE_CONSECUTIVE = 0 << 6,
    RLE_TYPE_REPEAT = 1 << 6,
    RLE_TYPE_SKIP = 2 << 6,
    RLE_TYPE_END = 3 << 6
};

#ifdef PLATFORM_DOS

extern "C"
{
    extern void __watcall drawRleSprite(char far* spriteData, char far* dst, uint16_t lineWidth);
}

#else

void drawRleSprite(char* spriteData, char* dst, uint16_t lineWidth);

#endif

#endif

