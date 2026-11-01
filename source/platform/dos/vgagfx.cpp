#include "vgagfx.h"

#include <stdio.h>
#include <string.h>
#include <dos.h>
#include <conio.h>

#include "rgbi_colors.h"
#include "palette_tools.h"
#include "convert_to_tga.h"

#define inportb(px) inp(px)


static const uint16_t DEATH_ANIMATION_PALETTE_FRAMES = 16;
static const uint16_t DEATH_ANIMATION_PALETTE_ENTRIES = 16;
static const uint16_t DEATH_ANIMATION_PALETTE_BYTES = DEATH_ANIMATION_PALETTE_ENTRIES * 3;


// unsigned, so that all offset calculations are done with fast 16 bit arithmetic
#define SCREEN_W 320u
#define SCREEN_H 200u


#define CGA_COLOR_COUNT 16


extern void videoInit(uint8_t mode);
#pragma aux videoInit =    \
    "mov ah, 0"            \
    "int 10h"              \
    modify [ax]            \
    parm [al];

/**
 * Copies a rectangular block of bytes between two buffers with a line pitch of SCREEN_W bytes.
 * Width and height must not be 0.
 */
extern void copyRect(char far* dest, const char far* src, uint16_t width, uint16_t height);
#pragma aux copyRect =         \
    "push ds"                  \
    "mov ds, dx"               \
    "mov dx, 320"              \
    "sub dx, ax"               \
    "cld"                      \
    "copy_line:"               \
    "mov cx, ax"               \
    "shr cx, 1"                \
    "rep movsw"                \
    "adc cx, cx"               \
    "rep movsb"                \
    "add si, dx"               \
    "add di, dx"               \
    "dec bx"                   \
    "jnz copy_line"            \
    "pop ds"                   \
    parm [es di] [dx si] [ax] [bx] \
    modify [bx cx dx si di es];

/**
 * Writes the 16 RGBI colors to the VGA DAC, starting at the current DAC write index.
 */
static void writeRgbiPalette()
{
    for (int i = 0; i < 16; ++i)
    {
        // Colors need to be set in RGB order. As rgbiColors is in BGR order, we need to reverse the order.
        // As VGA only supports 6 bits per color, we need to shift the values by 2 bits.
        outp(0x3C9, rgbiColors[i * 3 + 2] >> 2);
        outp(0x3C9, rgbiColors[i * 3 + 1] >> 2);
        outp(0x3C9, rgbiColors[i * 3 + 0] >> 2);
    }
}

/**
 * Sets the default VGA palette.
 * 
 * Sets the first 16 colors to the default VGA palette which is the same as the CGA and EGA palette.
 * Normally this is already set by the graphics card, but some games may change it.
 */
void setDefaultVgaPalette()
{
    outp(0x3C8, 0);
    writeRgbiPalette();

    // set the following 16 colors to an inverted grayscale versions of the first 16 colors
    // This pallette is used by the death animation of the game.
    for (int i = 0; i < 16; ++i)
    {
        // integer version of "* 0.75", avoids slow floating point emulation
        uint8_t gray = ((255 - rgbToGray(rgbiColors[i * 3 + 2], rgbiColors[i * 3 + 1], rgbiColors[i * 3 + 0])) * 3) >> 2;
        outp(0x3C9, gray >> 2);
        outp(0x3C9, gray >> 2);
        outp(0x3C9, gray >> 2);
    }
}

void setInBetweenPalette(uint8_t frame, uint8_t* paletteAnimation)
{
    if (frame >= DEATH_ANIMATION_PALETTE_FRAMES)
    {
        return;
    }
    outp(0x3C8, 0);
    for (int i = frame * DEATH_ANIMATION_PALETTE_BYTES;
         i < (frame + 1) * DEATH_ANIMATION_PALETTE_BYTES;
         ++i)
    {
        outp(0x3C9, paletteAnimation[i]);
    }
}

void createDeathAnimationPaletteAnimation(uint8_t* paletteAnimation)
{
    uint8_t grays[DEATH_ANIMATION_PALETTE_ENTRIES];
    for (int i = 0; i < DEATH_ANIMATION_PALETTE_ENTRIES; ++i)
    {
        grays[i] = rgbToGray(rgbiColors[i * 3 + 2], rgbiColors[i * 3 + 1], rgbiColors[i * 3 + 0]);
    }

    // Blends from color to gray using integer math only, as floating point is emulated in software.
    uint16_t pos = 0;
    for (uint16_t frame = 0; frame < DEATH_ANIMATION_PALETTE_FRAMES; ++frame)
    {
        uint16_t grayFactor = frame;
        uint16_t colorFactor = DEATH_ANIMATION_PALETTE_FRAMES - frame;

        for (int i = 0; i < DEATH_ANIMATION_PALETTE_ENTRIES; ++i)
        {
            uint8_t gray = (grays[i] * grayFactor) / DEATH_ANIMATION_PALETTE_FRAMES;
            paletteAnimation[pos++] = (gray + (rgbiColors[i * 3 + 2] * colorFactor) / DEATH_ANIMATION_PALETTE_FRAMES) >> 2;
            paletteAnimation[pos++] = (gray + (rgbiColors[i * 3 + 1] * colorFactor) / DEATH_ANIMATION_PALETTE_FRAMES) >> 2;
            paletteAnimation[pos++] = (gray + (rgbiColors[i * 3 + 0] * colorFactor) / DEATH_ANIMATION_PALETTE_FRAMES) >> 2;
        }
    }
}

VgaGfx::VgaGfx() :
    m_greyFramesLeft(0)
{
    m_greyPaletteAnimation = new uint8_t[DEATH_ANIMATION_PALETTE_BYTES * DEATH_ANIMATION_PALETTE_FRAMES];
    createDeathAnimationPaletteAnimation(m_greyPaletteAnimation);
    uint16_t screenBytes = SCREEN_W * SCREEN_H;

    m_backgroundImage = new char[screenBytes];
    m_screenBuffer = new char[screenBytes];
    
    memset(m_backgroundImage, 0xff, screenBytes);
    memset(m_screenBuffer, 0x0, screenBytes);

    Rectangle rect(0, 0, SCREEN_W, SCREEN_H);
    m_dirtyRects.push_back(rect);

    videoInit(0x13);

    setDefaultVgaPalette();
}

VgaGfx::~VgaGfx()
{
    delete[] m_greyPaletteAnimation;
    delete[] m_screenBuffer;
    delete[] m_backgroundImage;

    videoInit(0x03);
}

#define CGA_STATUS_REG 0x03DA
#define VERTICAL_RETRACE_BIT 0x08

#define waitForVsync() \
    do { \
        while (inportb(CGA_STATUS_REG) & VERTICAL_RETRACE_BIT); \
        while (!(inportb(CGA_STATUS_REG) & VERTICAL_RETRACE_BIT)); \
    } while (0)

#define LINE_BYTES SCREEN_W

static char far* screen = (char far*)(0xA0000000L);

#define getScreenLine(line) (screen + LINE_BYTES * (line))

#define getRectOffset(rect) ((uint16_t)(rect).y * SCREEN_W + (uint16_t)(rect).x)

void VgaGfx::vsync()
{
    waitForVsync();
}


void VgaGfx::draw(const Drawable& image, int16_t x, int16_t y)
{
    Rectangle rect(
        x, y,
        image.width(), image.height());
    rect = rect.intersection(Rectangle(0, 0, SCREEN_W, SCREEN_H));
    if (!rect.empty())
    {
        m_dirtyRects.push_back(rect);
        m_undrawnRects.push_back(rect);
    }

    image.draw(m_screenBuffer, SCREEN_W, SCREEN_H, x, y);
}

void VgaGfx::drawBackground(const Drawable& image, int16_t x, int16_t y)
{
    Rectangle rect(
        x, y,
        image.width(), image.height());
    rect = rect.intersection(Rectangle(0, 0, SCREEN_W, SCREEN_H));
    if (!rect.empty())
    {
        m_dirtyRects.push_back(rect);
    }

    image.draw(m_backgroundImage, SCREEN_W, SCREEN_H, x, y);
}


void VgaGfx::clear()
{
    for (int i = 0; i < m_dirtyRects.size(); ++i)
    {
        const Rectangle& rect = m_dirtyRects[i];
        if (rect.empty())
        {
            continue;
        }
        m_undrawnRects.push_back(rect);
        uint16_t offset = getRectOffset(rect);
        copyRect(m_screenBuffer + offset, m_backgroundImage + offset, rect.width, rect.height);
    }
    m_dirtyRects.clear();
}

void VgaGfx::drawScreen()
{   
    vsync();
    if (m_greyFramesLeft > 0)
    {
        // decrement first, so that the last step sets frame 0, which is the original palette
        --m_greyFramesLeft;
        setInBetweenPalette(m_greyFramesLeft, m_greyPaletteAnimation);
    }

    // todo: Create minimal set of rectangles to redraw.

    for (int i = 0; i < m_undrawnRects.size(); ++i)
    {
        const Rectangle& rect = m_undrawnRects[i];
        if (rect.empty())
        {
            continue;
        }
        uint16_t offset = getRectOffset(rect);
        copyRect(screen + offset, m_screenBuffer + offset, rect.width, rect.height);
    }

    m_undrawnRects.clear();
}


void VgaGfx::setBackground(const ImageBase& image)
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

void VgaGfx::saveAsTgaImage(const char* filename)
{
    convertToTga(screen, filename);
}

/**
 * Adds delta to every word of the screen, waiting for vsync every 15 lines.
 *
 * Processing two pixels at once is safe, as long as adding / subtracting 16
 * does not overflow / underflow the low byte, which is the case for the color ranges 0-31 used here.
 */
static void addToScreenWords(uint16_t delta)
{
    for (uint16_t y = 0; y < SCREEN_H; ++y)
    {
        if ((y % 15) == 0) waitForVsync();
        uint16_t far* pos = (uint16_t far*)getScreenLine(y);
        uint16_t far* lineEnd = pos + SCREEN_W / 2;

        for (; pos != lineEnd; ++pos)
        {
            *pos += delta;
        }
    }
}

uint32_t VgaGfx::drawDeathEffect()
{
    // switch to the inverted grayscale colors 16-31
    addToScreenWords(0x1010);
    setInBetweenPalette(DEATH_ANIMATION_PALETTE_FRAMES - 1, m_greyPaletteAnimation);
    m_greyFramesLeft = DEATH_ANIMATION_PALETTE_FRAMES - 1;
    // switch back to colors 0-15, which now hold the gray palette
    addToScreenWords((uint16_t)-0x1010);

    return 0;
}

void setFancyWipePalette()
{
    // colors 0-15 and 16-31 both get the normal RGBI palette
    outp(0x3C8, 0);
    writeRgbiPalette();
    writeRgbiPalette();
}


void createFadeToBlackPalette(uint8_t* palette, uint8_t frameCount, uint8_t frame)
{
    for (int i = 0; i < CGA_COLOR_COUNT; ++i)
    {
        palette[i * 3 + 0] = ((uint16_t)rgbiColors[i * 3 + 0] * frame) / frameCount;
        palette[i * 3 + 1] = ((uint16_t)rgbiColors[i * 3 + 1] * frame) / frameCount;
        palette[i * 3 + 2] = ((uint16_t)rgbiColors[i * 3 + 2] * frame) / frameCount;
    }
}


void fadeSecondPalette(uint8_t frameCount, bool fadeToBlack)
{
    uint8_t palette[CGA_COLOR_COUNT * 3];
    for (int frame = 0; frame < frameCount; ++frame)
    {
        // fade out goes from (frameCount - 1) / frameCount down to black,
        // fade in goes from 1 / frameCount up to full brightness
        createFadeToBlackPalette(palette, frameCount, fadeToBlack?(frameCount - frame - 1):(frame + 1));
        waitForVsync();
        outp(0x3C8, 16);
        for (int i = 0; i < CGA_COLOR_COUNT; ++i)
        {
            outp(0x3C9, palette[i * 3 + 2] >> 2);
            outp(0x3C9, palette[i * 3 + 1] >> 2);
            outp(0x3C9, palette[i * 3 + 0] >> 2);
        }
    }
}


#define randomShuffleLength 200
static const uint8_t randomShuffle[] = {
    176, 85, 48, 86, 60, 199, 173, 179, 198, 16, 20, 132, 172, 81, 46, 62, 144, 186, 158, 182, 196, 162, 59, 28, 38, 76, 188, 98, 89, 168, 37, 180, 44, 113, 165, 122, 51, 90, 112, 11, 35, 138, 142, 139, 55, 178, 75, 134, 50, 49, 161, 88, 80, 159, 7, 2, 130, 195, 116, 47, 42, 8, 187, 152, 74, 23, 57, 171, 97, 36, 127, 18, 40, 33, 177, 6, 39, 53, 117, 67, 64, 58, 83, 114, 92, 154, 184, 69, 19, 137, 94, 174, 65, 31, 10, 157, 9, 96, 129, 43, 30, 41, 119, 123, 25, 15, 66, 167, 12, 13, 136, 118, 22, 155, 111, 106, 121, 104, 147, 140, 105, 133, 32, 45, 100, 156, 135, 110, 52, 29, 61, 3, 181, 150, 79, 84, 143, 72, 73, 185, 166, 108, 56, 93, 26, 82, 14, 160, 189, 153, 0, 109, 24, 70, 63, 68, 5, 1, 101, 54, 192, 4, 170, 107, 169, 131, 141, 27, 193, 197, 151, 91, 124, 102, 21, 191, 77, 190, 87, 183, 34, 103, 71, 95, 126, 145, 149, 148, 99, 194, 78, 128, 164, 146, 120, 163, 17, 175, 125, 115
};


void calcNthRectangle(Rectangle& rect, uint16_t n)
{
    rect.width = 16;
    rect.height = 20;

    rect.x = (n * rect.width) % SCREEN_W;
    rect.y = (n * rect.width) / SCREEN_W * rect.height;
}

#define SCREEN_FADE_FRAMES 7
#define TILES_AT_A_TIME 8

void VgaGfx::fancyWipe(const ImageBase& image)
{
    if (image.width() != SCREEN_W ||
        image.height() != SCREEN_H)
    {
        return;
    }

    setFancyWipePalette();

    char* imgData = image.data();
    // Tiles are 16 pixels wide and start at x positions divisible by 16, so two pixels can be processed at once.
    // Adding 16 to each byte is safe, as the colors are in the range 0-15.
    uint16_t far* screenWords = (uint16_t far*)screen;
    const uint16_t far* imgWords = (const uint16_t far*)imgData;

    uint16_t rectangleNumber = 0;

    for (uint16_t loop = 0; loop < (200 / TILES_AT_A_TIME); ++loop)
    {

        // first fade out the old image
        for (uint16_t n = 0; n < TILES_AT_A_TIME; ++n)
        {
            Rectangle rect;
            calcNthRectangle(rect, randomShuffle[rectangleNumber + n]);
            uint16_t offset = getRectOffset(rect) / 2;
            for (uint16_t y = 0; y < rect.height; ++y)
            {
                for (uint16_t x = 0; x < rect.width / 2; ++x)
                {
                    screenWords[offset + x] += 0x1010;
                }
                offset += SCREEN_W / 2;
            }
        }
        fadeSecondPalette(SCREEN_FADE_FRAMES, true);

        // now copy the new image
        for (uint16_t n = 0; n < TILES_AT_A_TIME; ++n)
        {
            Rectangle rect;
            calcNthRectangle(rect, randomShuffle[rectangleNumber + n]);
            uint16_t offset = getRectOffset(rect) / 2;
            for (uint16_t y = 0; y < rect.height; ++y)
            {
                for (uint16_t x = 0; x < rect.width / 2; ++x)
                {
                    screenWords[offset + x] = imgWords[offset + x] + 0x1010;
                }
                offset += SCREEN_W / 2;
            }
        }
        fadeSecondPalette(SCREEN_FADE_FRAMES, false);

        // now fix colors of copied data
        for (uint16_t n = 0; n < TILES_AT_A_TIME; ++n)
        {
            Rectangle rect;
            calcNthRectangle(rect, randomShuffle[rectangleNumber + n]);
            uint16_t offset = getRectOffset(rect) / 2;
            for (uint16_t y = 0; y < rect.height; ++y)
            {
                for (uint16_t x = 0; x < rect.width / 2; ++x)
                {
                    screenWords[offset + x] -= 0x1010;
                }
                offset += SCREEN_W / 2;
            }
        }

        rectangleNumber += TILES_AT_A_TIME;
    }

    // restore normal palette
    setDefaultVgaPalette();

    // keep the buffers in sync with what is now on screen
    memcpy(m_backgroundImage, imgData, SCREEN_W * SCREEN_H);
    memcpy(m_screenBuffer, imgData, SCREEN_W * SCREEN_H);
    m_dirtyRects.clear();
    m_undrawnRects.clear();
}