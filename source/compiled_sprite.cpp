#include "compiled_sprite.h"

#include "tga_image.h"
#include "log.h"

#include <string.h>

void write8Bit(char* function, int& functionPos, uint16_t offset, uint8_t data)
{
    strcpy(function + functionPos, "\xC6\x87"); // mov    byte ptr offset[bx],data
    functionPos += 2;

    memcpy(function + functionPos, &offset, 2);
    functionPos += 2;

    *(function + functionPos) = data;
    functionPos += 1;
}

void write16Bit(char* function, int& functionPos, uint16_t offset, uint16_t data)
{
    strcpy(function + functionPos, "\xC7\x87"); // mov    byte ptr offset[bx],data
    functionPos += 2;

    memcpy(function + functionPos, &offset, 2);
    functionPos += 2;

    memcpy(function + functionPos, &data, 2);
    functionPos += 2;
}

DrawCompiledSpritePtr CompiledSprite::compileSprite(const PixelSource& image, int16_t targetWidth)
{
    int functionSize = 0;
    for (int y = 0; y < image.height(); ++y)
    {
        int consecutivePixel = 0;
        for (int x = 0; x < image.width(); ++x)
        {
            if (image.pixel(x,y) == image.transparentColor())
            {
                if (consecutivePixel == 1)
                {
                    functionSize += 5;
                    consecutivePixel = 0;
                }
            }
            else
            {
                if (consecutivePixel == 0) consecutivePixel++;
                else
                {
                    functionSize += 6;
                    consecutivePixel = 0;
                }
                
            }
        }
        if (consecutivePixel != 0) functionSize += 5;
    }

    char* function = new char[functionSize + 10];
    int functionPos = 0;

    const char* functionHeader =
        "\x53"      // push bx
        "\x89\xc3"  // mov bx,ax
        "\x8E\xDA"; // mov ds,dx

    const char* functionEnd =
        "\x5b"      // pop bx
        "\xcb";     // retf

    strcpy(function, functionHeader);
    functionPos = strlen(function);


    char lastPixel;

    for (int y = 0; y < image.height(); ++y)
    {
        int consecutivePixel = 0;
        for (int x = 0; x < image.width(); ++x)
        {
            char pixel = image.pixel(x,y);
            if (pixel == 0) // if pixel is transparent
            {
                if (consecutivePixel == 1)
                {
                    write8Bit(function, functionPos, targetWidth * y + x - 1, lastPixel);
                    consecutivePixel = 0;
                }
            }
            else
            {
                if (consecutivePixel == 0) consecutivePixel++;
                else
                {
                    write16Bit(function, functionPos, targetWidth * y + x - 1, lastPixel | pixel << 8);
                    consecutivePixel = 0;
                }
                
            }
            lastPixel = pixel;
        }
        if (consecutivePixel != 0)
        {
            write8Bit(function, functionPos, targetWidth * y + image.width() - 1, lastPixel);
        }
    }

    strcpy(function + functionPos, functionEnd);

    return (DrawCompiledSpritePtr)function;
}

void CompiledSprite::freeCompiledSprite(DrawCompiledSpritePtr sprite)
{
    delete (char*)sprite;
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


CompiledSprite::~CompiledSprite()
{
    freeCompiledSprite(m_compiledFunction);
}

int16_t CompiledSprite::width() const
{
    return m_width;
}

int16_t CompiledSprite::height() const
{
    return m_height;
}

/*
void sprite2(char* img)
{
    img[0 * 320 + 6] = 4;
    img[1 * 320 + 3] = 4;
}
*/

void CompiledSprite::draw(char* target, int16_t targetWidth, int16_t targetHeight, int16_t targetX, int16_t targetY) const
{
    char* img = target + targetWidth * targetY + targetX;
    m_compiledFunction(img);
}
