#include "framebuffer_gfx.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "exception.h"

#include "rgbi_colors.h"
#include "palette_tools.h"
#include "convert_to_tga.h"

static const uint16_t DEATH_ANIMATION_PALETTE_FRAMES = 16;
static const uint16_t DEATH_ANIMATION_PALETTE_ENTRIES = 16;
static const uint16_t DEATH_ANIMATION_PALETTE_BYTES = DEATH_ANIMATION_PALETTE_ENTRIES * 3;

static const uint16_t COLOR_REGENERATION_FRAMES = 24;

#define SCREEN_W 320L
#define SCREEN_H 200L


#define CGA_COLOR_COUNT 16


const uint32_t DEATH_ANIMATION_FRAMES = 28;

int compareRectangles(const void* a, const void* b) {
   return ( ((Rectangle*)a)->y > ((Rectangle*)b)->y );
}


void sortRects(tnd::vector<Rectangle>& rects)
{
    qsort(&rects[0], rects.size(), sizeof(Rectangle), compareRectangles);
}

void setDefaultPalette(uint32_t* palette)
{
    for (int i = 0; i < CGA_COLOR_COUNT; ++i)
    {
        palette[i] = 
            (rgbiColors[i * 3 + 2] << 24) | // Red
            (rgbiColors[i * 3 + 1] << 16) | // Green
            (rgbiColors[i * 3 + 0] <<  8) | // Blue
            0xFF /* alpha */;
    }
}

enum GrayscalePaletteType
{
    NON_INVERTED = 1,
    INVERTED = 2
};

void setGrayscalePalette(uint32_t* palette, GrayscalePaletteType inverted)
{
    for (int i = 0; i < CGA_COLOR_COUNT; ++i)
    {
        auto transform = (inverted == INVERTED) ?
            [](uint8_t val)->uint8_t{ return 255 - val;} :
            [](uint8_t val)->uint8_t{ return val;};

        uint8_t gray = transform(rgbToGray(rgbiColors[i * 3 + 2], rgbiColors[i * 3 + 1], rgbiColors[i * 3 + 0]));

        palette[i] = 
            (gray << 24) | // Red
            (gray << 16) | // Green
            (gray <<  8) | // Blue
            0xFF /* alpha */;
    }
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
    setDefaultPalette(m_rgbiColorsToRgba8888.data());
    
    // set the following 16 colors to an inverted grayscale versions of the first 16 colors
    // This pallette is used by the death animation of the game.
    setGrayscalePalette(m_rgbiColorsToRgba8888.data() + CGA_COLOR_COUNT, INVERTED);
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

void transitionToNormalPalette(uint32_t* palette, uint8_t frameCount, uint8_t frame)
{
    // generate a grayscale palette
    std::array<uint32_t, CGA_COLOR_COUNT> grayPalette;
    setGrayscalePalette(grayPalette.data(), NON_INVERTED);

    // generate a normal palette
    std::array<uint32_t, CGA_COLOR_COUNT> normalPalette;
    setDefaultPalette(normalPalette.data());

    // generate a transition palette
    for (int i = 0; i < CGA_COLOR_COUNT; ++i)
    {
        uint8_t grayR = (grayPalette[i] >> 24) & 0xFF;
        uint8_t grayG = (grayPalette[i] >> 16) & 0xFF;
        uint8_t grayB = (grayPalette[i] >> 8) & 0xFF;

        uint8_t normalR = (normalPalette[i] >> 24) & 0xFF;
        uint8_t normalG = (normalPalette[i] >> 16) & 0xFF;
        uint8_t normalB = (normalPalette[i] >> 8) & 0xFF;

        uint8_t transitionR = grayR + ((normalR - grayR) * frame / frameCount);
        uint8_t transitionG = grayG + ((normalG - grayG) * frame / frameCount);
        uint8_t transitionB = grayB + ((normalB - grayB) * frame / frameCount);

        palette[i] =
            (transitionR << 24) |
            (transitionG << 16) |
            (transitionB << 8) |
            0xFF /* alpha */;
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
            m_colorRegenerationFramesLeft = COLOR_REGENERATION_FRAMES;
            // setGrayscalePalette(m_rgbiColorsToRgba8888.data(), NON_INVERTED);
            int effectiveLeft = m_deathEffectFramesLeft - 1;
            int startLine = effectiveLeft * blockHeight;
            int endLine = (effectiveLeft + 1) * blockHeight;
            addToBlock(m_screenBuffer, startLine, endLine, -16);
        }
        m_deathEffectFramesLeft--;
    }

    if (m_colorRegenerationFramesLeft > 0)
    {
        m_colorRegenerationFramesLeft--;
        transitionToNormalPalette(m_rgbiColorsToRgba8888.data(), COLOR_REGENERATION_FRAMES, COLOR_REGENERATION_FRAMES - m_colorRegenerationFramesLeft);
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
        THROW_EXCEPTION("FramebufferGfx::renderToMemory: Unsupported pixel format");
    }
}

uint32_t FramebufferGfx::drawDeathEffect()
{
    m_deathEffectFramesLeft = DEATH_ANIMATION_FRAMES;
    return m_deathEffectFramesLeft;
}

void FramebufferGfx::fancyWipe(const ImageBase& image)
{
}

int16_t FramebufferGfx::getScreenWidth() const
{
    return SCREEN_W;
}

int16_t FramebufferGfx::getScreenHeight() const
{
    return SCREEN_H;
}