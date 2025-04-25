#include "framebuffer_gfx.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "exception.h"

#include "rgbi_colors.h"
#include "palette_tools.h"

static const uint16_t DEATH_ANIMATION_PALETTE_FRAMES = 16;
static const uint16_t DEATH_ANIMATION_PALETTE_ENTRIES = 16;
static const uint16_t DEATH_ANIMATION_PALETTE_BYTES = DEATH_ANIMATION_PALETTE_ENTRIES * 3;


#define SCREEN_W 320L
#define SCREEN_H 200L


#define CGA_COLOR_COUNT 16


const uint32_t DEATH_ANIMATION_FRAMES = 32;

int compareRectangles(const void* a, const void* b) {
   return ( ((Rectangle*)a)->y > ((Rectangle*)b)->y );
}


void sortRects(tnd::vector<Rectangle>& rects)
{
    qsort(&rects[0], rects.size(), sizeof(Rectangle), compareRectangles);
}

FramebufferGfx::FramebufferGfx()
{
    long int screenBytes = SCREEN_W * SCREEN_H;

    m_backgroundImage = new char[screenBytes];
    m_screenBuffer = new char[screenBytes];
    
    memset(m_backgroundImage, 0xff, screenBytes);
    memset(m_screenBuffer, 0x0, screenBytes);

    Rectangle rect(0, 0, SCREEN_W, SCREEN_H);
    m_dirtyRects.push_back(rect);

    // Initialize the palette
    for (int i = 0; i < CGA_COLOR_COUNT; ++i)
    {
        m_rgbiColorsToRgba8888[i] = 
            (rgbiColors[i * 3 + 2] << 24) | // Red
            (rgbiColors[i * 3 + 1] << 16) | // Green
            (rgbiColors[i * 3 + 0] <<  8) | // Blue
            0xFF /* alpha */;
    }

    // set the following 16 colors to an inverted grayscale versions of the first 16 colors
    // This pallette is used by the death animation of the game.
    for (int i = 0; i < CGA_COLOR_COUNT; ++i)
    {
        uint8_t gray = 255 - rgbToGray(rgbiColors[i * 3 + 2], rgbiColors[i * 3 + 1], rgbiColors[i * 3 + 0]);

        m_rgbiColorsToRgba8888[i + CGA_COLOR_COUNT] = 
            (gray << 24) | // Red
            (gray << 16) | // Green
            (gray <<  8) | // Blue
            0xFF /* alpha */;
    }
}

FramebufferGfx::~FramebufferGfx()
{
    delete[] m_screenBuffer;
    delete[] m_backgroundImage;
}

#define LINE_BYTES SCREEN_W

#define getBackBufferLine(line) (m_screenBuffer + LINE_BYTES * (line))
#define getBackgroundImageLine(line) (m_backgroundImage + LINE_BYTES * (line))

void FramebufferGfx::vsync()
{
    
}

void FramebufferGfx::draw(const Drawable& image, int16_t x, int16_t y)
{
    Rectangle rect(
        x, y,
        image.width(), image.height());
    rect = rect.intersection(Rectangle(0, 0, SCREEN_W, SCREEN_H));
    m_dirtyRects.push_back(rect);

    image.draw(m_screenBuffer, SCREEN_W, SCREEN_H, x, y);
}

void FramebufferGfx::drawBackground(const Drawable& image, int16_t x, int16_t y)
{
    Rectangle rect(
        x, y,
        image.width(), image.height());
    rect = rect.intersection(Rectangle(0, 0, SCREEN_W, SCREEN_H));
    m_dirtyRects.push_back(rect);

    image.draw(m_backgroundImage, SCREEN_W, SCREEN_H, x, y);
}


void FramebufferGfx::clear()
{
    for (int i = 0; i < m_dirtyRects.size(); ++i)
    {
        const Rectangle& rect = m_dirtyRects[i];
        int16_t rectHeight = rect.height;
        int16_t rectWidth = rect.width;
        int16_t rectY = rect.y;
        int16_t rectX = rect.x;
        for (int16_t y = 0; y < rectHeight; ++y)
        {
            memcpy(getBackBufferLine(y + rectY) + rectX, getBackgroundImageLine(y + rectY) + rectX, rectWidth);
        }
    }
    m_dirtyRects.clear();
}


void addToBlock(char* screenBuffer, uint16_t startLine, uint16_t endLine, int16_t value)
{
    if (endLine > SCREEN_H) endLine = SCREEN_H;
    for (uint16_t y = startLine; y < endLine; ++y)
    {
        char* line = screenBuffer + y * SCREEN_W;
        for (uint16_t x = 0; x < SCREEN_W; ++x)
        {
            line[x] += value;
        }
    }
}

void FramebufferGfx::drawScreen()
{
    // death effect animation
    if (m_deathEffectFramesLeft > 0)
    {
        int halfDeathEffectFrames = DEATH_ANIMATION_FRAMES / 2;
        int blockHeight = SCREEN_H / (halfDeathEffectFrames - 1);

        if (m_deathEffectFramesLeft > halfDeathEffectFrames)
        {
            int effectiveLeft = m_deathEffectFramesLeft - halfDeathEffectFrames - 1;
            int startLine = effectiveLeft * blockHeight;
            int endLine = (effectiveLeft + 1) * blockHeight;
            addToBlock(m_screenBuffer, startLine, endLine, 16);
        }
        else
        {
            int effectiveLeft = m_deathEffectFramesLeft - 1;
            int startLine = effectiveLeft * blockHeight;
            int endLine = (effectiveLeft + 1) * blockHeight;
            addToBlock(m_screenBuffer, startLine, endLine, -16);
        }
        m_deathEffectFramesLeft--;
    }
}

void FramebufferGfx::setBackground(const ImageBase& image)
{
    if (image.width() == SCREEN_W &&
        image.height() == SCREEN_H)
    {
        memcpy(m_backgroundImage, image.data(), SCREEN_W * SCREEN_H);
        Rectangle rect(
            0, 0,
            image.width(), image.height());
        m_dirtyRects.push_back(rect);
    }
}



void write16bit(uint16_t data, FILE* fp)
{
    fwrite(&data, 2, 1, fp);
}

void write8bit(uint8_t data, FILE* fp)
{
    fwrite(&data, 1, 1, fp);
}

void convertToTga(char* data, const char* filename)
{
    FILE* fp = fopen(filename, "wb");

    write8bit(0, fp); // id length in bytes
    write8bit(1, fp); // color map type (1 == color map / palette present)
    write8bit(1, fp); // image type (1 == uncompressed color-mapped image)
    write16bit(0, fp); // color map origin
    write16bit(16, fp); // color map length
    write8bit(24, fp); // color map depth (bits per palette entry)
    write16bit(0, fp); // x origin
    write16bit(0, fp); // y origin
    write16bit(320, fp); // image width
    write16bit(200, fp); // image height
    write8bit(8, fp); // bits per pixel
    write8bit((1 << 5), fp); // image descriptor

    // we do not have id data, so do not write any
    fwrite(rgbiColors, 1, 16 * 3, fp);

    // write uncompressed
    fwrite(data, 1, 320 * 200, fp);
    
    fclose(fp);
}

void FramebufferGfx::saveAsTgaImage(const char* filename)
{
    convertToTga(m_screenBuffer, filename);
}

void FramebufferGfx::renderToMemory(void *buffer, uint32_t pitch, PixelFormat format)
{
    if (format == PIXEL_FORMAT_RGBA8888)
    {
        char* rgbaBuffer = (char*)buffer;
        for (int y = 0; y < SCREEN_H; ++y)
        {
            char* screenLine = m_screenBuffer + y * SCREEN_W;
            uint32_t* rgbaLine = (uint32_t*)(rgbaBuffer + y * pitch);
            for (int x = 0; x < SCREEN_W; ++x)
            {  
                *rgbaLine = m_rgbiColorsToRgba8888[*screenLine];
                ++screenLine;
                ++rgbaLine;
            }
        }
    }
    else
    {
        // unsupported format, throw an exception
        throw Exception("FramebufferGfx::renderToMemory: Unsupported pixel format");
    }
}


uint32_t FramebufferGfx::drawDeathEffect()
{
    m_deathEffectFramesLeft = DEATH_ANIMATION_FRAMES;
    return m_deathEffectFramesLeft;
}

void createFadeToBlackPalette(uint8_t* palette, uint8_t frameCount, uint8_t frame)
{
   
}

void FramebufferGfx::fancyWipe(const ImageBase& image)
{
}

void FramebufferGfx::renderToRgb(char* rgbBuffer) const
{
    // convert to 24 bit rgb
    for (int y = 0; y < SCREEN_H; ++y)
    {
        for (int x = 0; x < SCREEN_W; ++x)
        {
            char pixel = m_screenBuffer[y * SCREEN_W + x];
            char* rgb = rgbBuffer + (y * SCREEN_W + x) * 3;
            rgb[0] = rgbiColors[pixel * 3 + 2]; // B
            rgb[1] = rgbiColors[pixel * 3 + 1]; // G
            rgb[2] = rgbiColors[pixel * 3 + 0]; // R
        }
    }
}

int16_t FramebufferGfx::getScreenWidth() const
{
    return SCREEN_W;
}

int16_t FramebufferGfx::getScreenHeight() const
{
    return SCREEN_H;
}