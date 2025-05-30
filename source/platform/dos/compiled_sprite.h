#ifndef _COMPILED_SPRITE_H_INCLUDED
#define _COMPILED_SPRITE_H_INCLUDED

#include "drawable.h"
#include "pixel_source.h"

#include <stdint.h>


/**
 * A compiled sprite loads the given tga file and compiles it to
 * 8086 machine code. This makes drawing it faster, especially if
 * it contains transparency, bit it has an overhead of 2 to 4 bytes
 * per pixel in ram, depending on image content.
 * 
 * The target image width has to be known at construction time of the
 * sprite.
 * 
 * To get the assembler bytecodes I used the disassembler of
 * open watcom "wdis".
 */
class CompiledSprite : public Drawable
{
public:
    CompiledSprite(const char* filename, int16_t targetWidth);
    CompiledSprite(const PixelSource& image, int16_t targetWidth);
    virtual ~CompiledSprite();

    virtual int16_t width() const;
    virtual int16_t height() const;
    virtual void draw(char* target, int16_t targetWidth, int16_t targetHeight, int16_t targetX, int16_t targetY) const;

private:
    char* compileSprite(const PixelSource& image, int16_t targetWidth);

private:
    int16_t m_width, m_height;
    char* m_compiledFunction;
};

typedef void (*DrawCompiledSpritePtr)(char* img);
uint32_t compileData(char* dst, uint32_t dstSize, const PixelSource& image, int16_t targetWidth);

#endif