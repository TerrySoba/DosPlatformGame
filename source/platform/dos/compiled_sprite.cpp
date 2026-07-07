#include "compiled_sprite.h"

#include "tga_image.h"

#include <string.h>

const int WRITE_8BIT_SIZE = 5;
const int WRITE_16BIT_SIZE = 6;

bool usesNoDisp(uint16_t offset)
{
    return offset == 0;
}

bool usesDisp8(uint16_t offset)
{
    return offset <= 127;
}

int write8BitSize(uint16_t offset)
{
    if (usesNoDisp(offset))
    {
        return 3;
    }
    if (usesDisp8(offset))
    {
        return 4;
    }
    return WRITE_8BIT_SIZE;
}

int write16BitSize(uint16_t offset)
{
    if (usesNoDisp(offset))
    {
        return 4;
    }
    if (usesDisp8(offset))
    {
        return 5;
    }
    return WRITE_16BIT_SIZE;
}

void write8Bit(char* functionBuffer, size_t functionBufferSize, int& functionPos, uint16_t offset, uint8_t data)
{
    int opSize = write8BitSize(offset);
    if (functionPos + opSize + 1 >= functionBufferSize)
    {
        return;
    }

    if (usesNoDisp(offset))
    {
        memcpy(functionBuffer + functionPos, "\xC6\x07", 2); // mov byte ptr [bx],imm8
        functionPos += 2;

        *(functionBuffer + functionPos) = data;
        functionPos += 1;
    }
    else if (usesDisp8(offset))
    {
        int8_t disp = (int8_t)offset;
        memcpy(functionBuffer + functionPos, "\xC6\x47", 2); // mov byte ptr [bx+disp8],imm8
        functionPos += 2;

        *(functionBuffer + functionPos) = disp;
        functionPos += 1;

        *(functionBuffer + functionPos) = data;
        functionPos += 1;
    }
    else
    {
        memcpy(functionBuffer + functionPos, "\xC6\x87", 2); // mov byte ptr [bx+disp16],imm8
        functionPos += 2;

        memcpy(functionBuffer + functionPos, &offset, 2);
        functionPos += 2;

        *(functionBuffer + functionPos) = data;
        functionPos += 1;
    }
}

void write16Bit(char* functionBuffer, size_t functionBufferSize, int& functionPos, uint16_t offset, uint16_t data)
{
    int opSize = write16BitSize(offset);
    if (functionPos + opSize + 1 >= functionBufferSize)
    {
        return;
    }

    if (usesNoDisp(offset))
    {
        memcpy(functionBuffer + functionPos, "\xC7\x07", 2); // mov word ptr [bx],imm16
        functionPos += 2;

        memcpy(functionBuffer + functionPos, &data, 2);
        functionPos += 2;
    }
    else if (usesDisp8(offset))
    {
        int8_t disp = (int8_t)offset;
        memcpy(functionBuffer + functionPos, "\xC7\x47", 2); // mov word ptr [bx+disp8],imm16
        functionPos += 2;

        *(functionBuffer + functionPos) = disp;
        functionPos += 1;

        memcpy(functionBuffer + functionPos, &data, 2);
        functionPos += 2;
    }
    else
    {
        memcpy(functionBuffer + functionPos, "\xC7\x87", 2); // mov word ptr [bx+disp16],imm16
        functionPos += 2;

        memcpy(functionBuffer + functionPos, &offset, 2);
        functionPos += 2;

        memcpy(functionBuffer + functionPos, &data, 2);
        functionPos += 2;
    }
}

/**
 * Compiles the given image to 8086 machine code.
 * 
 * If dst is NULL, the function only returns the size of the compiled code and does not write anything.
 * 
 * dst: The buffer to write the compiled code to.
 * dstSize: The size of the buffer.
 * image: The image to compile.
 * targetWidth: The width of the target screen (usually 320).
 */
uint32_t compileData(char* dst, uint32_t dstSize, const PixelSource& image, int16_t targetWidth)
{
    int functionSize = 0;
    int functionPos = 0;
    char lastPixel = 0;

    const char* functionHeader =
        "\x93"      // xchg ax,bx
        "\x8E\xDA"; // mov ds,dx

    const char* functionEnd =
        "\x93"      // xchg ax,bx
        "\xcb";     // retf

    char transparentColor = image.transparentColor();
    size_t functionBufferSize = 0;
            
    if (dst) {
        strlcpy(dst, functionHeader, dstSize);
        functionPos = strlen(dst);
    }

    for (int y = 0; y < image.height(); ++y)
    {
        int consecutivePixel = 0;
        for (int x = 0; x < image.width(); ++x)
        {
            char pixel = image.pixel(x, y);
            if (pixel == transparentColor)
            {
                if (consecutivePixel == 1)
                {
                    uint16_t offset = targetWidth * y + x - 1;
                    functionSize += write8BitSize(offset);
                    if (dst)
                    {
                        write8Bit(dst, dstSize, functionPos, offset, lastPixel);
                    }
                    consecutivePixel = 0;
                }
            }
            else
            {
                if (consecutivePixel == 0)
                    consecutivePixel++;
                else
                {
                    uint16_t offset = targetWidth * y + x - 1;
                    functionSize += write16BitSize(offset);
                    if (dst)
                    {
                        write16Bit(dst, dstSize, functionPos, offset, lastPixel | pixel << 8);
                    }
                    consecutivePixel = 0;
                }
            }
            lastPixel = pixel;
        }
        if (consecutivePixel != 0)
        {
            uint16_t offset = targetWidth * y + image.width() - 1;
            functionSize += write8BitSize(offset);
            if (dst)
            {
                write8Bit(dst, dstSize, functionPos, offset, lastPixel);
            }
        }
    }

    if (dst)
    {
        strlcpy(dst + functionPos, functionEnd, dstSize - functionPos);
    }

    return functionSize + strlen(functionHeader) + strlen(functionEnd) + 1; //  +1 for null termination
}

char* CompiledSprite::compileSprite(const PixelSource& image, int16_t targetWidth)
{
    size_t compiledSpriteSize = compileData(NULL, 0, image, targetWidth);
    char* compiledSprite = new char[compiledSpriteSize];
    (void)compileData(compiledSprite, compiledSpriteSize, image, targetWidth);
    return compiledSprite;
}

class MyPixelSource : public PixelSource
{
public:
    MyPixelSource(const char* data, int16_t width, int16_t height) :
        m_data(data), m_width(width), m_height(height)
    {
    }

    virtual int16_t width() const
    {
        return m_width;
    }

    virtual int16_t height() const
    {
        return m_height;
    }

    virtual char pixel(int16_t x, int16_t y) const
    {
        return m_data[m_width * y + x];
    }

    virtual char transparentColor() const 
    {
        return 0;
    }

private:
    const char* m_data;
    int16_t m_width, m_height;
};


CompiledSprite::CompiledSprite(const char* filename, int16_t targetWidth)
{
    TgaImage img(filename);
    m_width = img.width();
    m_height = img.height();
    MyPixelSource pixels(img.data(), img.width(), img.height());

    m_compiledFunction = compileSprite(pixels, targetWidth);
}

CompiledSprite::CompiledSprite(const PixelSource& image, int16_t targetWidth)
{
    m_width = image.width();
    m_height = image.height();
    m_compiledFunction = compileSprite(image, targetWidth);
}

CompiledSprite::~CompiledSprite()
{
    delete[] m_compiledFunction;
}

int16_t CompiledSprite::width() const
{
    return m_width;
}

int16_t CompiledSprite::height() const
{
    return m_height;
}


// void sprite2(char* img)
// {
//     img[0 * 320 + 6] = 4;
//     img[1 * 320 + 3] = 4;
// }


void CompiledSprite::draw(char* target, int16_t targetWidth, int16_t targetHeight, int16_t targetX, int16_t targetY) const
{
    char* img = target + targetWidth * targetY + targetX;
    ((DrawCompiledSpritePtr)m_compiledFunction)(img);
}
