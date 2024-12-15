#include "vgagfx.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dos.h>
#include <conio.h>
#include <stdio.h>

#define inport(px) inpw(px)
#define inportb(px) inp(px)

#define SCREEN_W 320L
#define SCREEN_H 200L


#define CGA_COLOR_COUNT 16

static const uint8_t rgbiColors[] = { // colors are in BGR order
    0x00,0x00,0x00,
    0xAA,0x00,0x00,
    0x00,0xAA,0x00,
    0xAA,0xAA,0x00,
    0x00,0x00,0xAA,
    0xAA,0x00,0xAA,
    0x00,0x55,0xAA,
    0xAA,0xAA,0xAA,
    0x55,0x55,0x55,
    0xFF,0x55,0x55,
    0x55,0xFF,0x55,
    0xFF,0xFF,0x55,
    0x55,0x55,0xFF,
    0xFF,0x55,0xFF,
    0x55,0xFF,0xFF,
    0xFF,0xFF,0xFF,
};

extern void videoInit(uint8_t mode);
#pragma aux videoInit =    \
    "mov ah, 0"            \
    "int 10h"              \
    modify [ax]            \
    parm [al];

int compareRectangles(const void* a, const void* b) {
   return ( ((Rectangle*)a)->y > ((Rectangle*)b)->y );
}


void sortRects(tnd::vector<Rectangle>& rects)
{
    qsort(&rects[0], rects.size(), sizeof(Rectangle), compareRectangles);
}


// convert RGB to grayscale according to rec601 luma
uint8_t rgbToGray(uint8_t r, uint8_t g, uint8_t b)
{
    // return 0.299 * r + 0.587 * g + 0.114 * b; // rec601 luma
    return 0.133 * r + 0.433 * g + 0.433 * b; // more artistic luma
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
    for (int i = 0; i < 16; ++i)
    {
        // Colors need to be set in RGB order. As rgbiColors is in BGR order, we need to reverse the order.
        // As VGA only supports 6 bits per color, we need to shift the values by 2 bits.
        outp(0x3C9, rgbiColors[i * 3 + 2] >> 2);
        outp(0x3C9, rgbiColors[i * 3 + 1] >> 2);
        outp(0x3C9, rgbiColors[i * 3 + 0] >> 2);
    }

    // set the following 16 colors to an inverted grayscale versions of the first 16 colors
    // This pallette is used by the death animation of the game.
    for (int i = 0; i < 16; ++i)
    {
        uint8_t gray = (255 - rgbToGray(rgbiColors[i * 3 + 2], rgbiColors[i * 3 + 1], rgbiColors[i * 3 + 0])) * 0.75;
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
    uint16_t pos = 0;
    for (int frame = 0; frame < DEATH_ANIMATION_PALETTE_FRAMES; ++frame)
    {
        float f1 = frame / (float)DEATH_ANIMATION_PALETTE_FRAMES;
        float f2 = 1.0f - f1;

        for (int i = 0; i < DEATH_ANIMATION_PALETTE_ENTRIES; ++i)
        {
            uint8_t gray = rgbToGray(rgbiColors[i * 3 + 2], rgbiColors[i * 3 + 1], rgbiColors[i * 3 + 0]);
            paletteAnimation[pos++] = (uint8_t(gray * f1) + uint8_t(rgbiColors[i * 3 + 2] * f2)) >> 2;
            paletteAnimation[pos++] = (uint8_t(gray * f1) + uint8_t(rgbiColors[i * 3 + 1] * f2)) >> 2;
            paletteAnimation[pos++] = (uint8_t(gray * f1) + uint8_t(rgbiColors[i * 3 + 0] * f2)) >> 2;
        }
    }
}

VgaGfx::VgaGfx() :
    m_greyFramesLeft(0)
{
    m_greyPaletteAnimation = new uint8_t[DEATH_ANIMATION_PALETTE_BYTES * DEATH_ANIMATION_PALETTE_FRAMES];
    createDeathAnimationPaletteAnimation(m_greyPaletteAnimation);
    long int screenBytes = SCREEN_W * SCREEN_H;

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
    while (inportb(CGA_STATUS_REG) & VERTICAL_RETRACE_BIT); \
    while (!(inportb(CGA_STATUS_REG) & VERTICAL_RETRACE_BIT))

#define LINE_BYTES SCREEN_W

static char far* screen = (char far*)(0xA0000000L);

#define getScreenLine(line) (screen + LINE_BYTES * (line))
#define getBackBufferLine(line) (m_screenBuffer + LINE_BYTES * (line))
#define getBackgroundImageLine(line) (m_backgroundImage + LINE_BYTES * (line))

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
    m_dirtyRects.push_back(rect);
    m_undrawnRects.push_back(rect);

    image.draw(m_screenBuffer, SCREEN_W, SCREEN_H, x, y);
}

void VgaGfx::drawBackground(const Drawable& image, int16_t x, int16_t y)
{
    Rectangle rect(
        x, y,
        image.width(), image.height());
    rect = rect.intersection(Rectangle(0, 0, SCREEN_W, SCREEN_H));
    m_dirtyRects.push_back(rect);

    image.draw(m_backgroundImage, SCREEN_W, SCREEN_H, x, y);
}


void VgaGfx::clear()
{
    for (int i = 0; i < m_dirtyRects.size(); ++i)
    {
        m_undrawnRects.push_back(m_dirtyRects[i]);
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

void VgaGfx::drawScreen()
{   
    vsync();
    if (m_greyFramesLeft > 0)
    {
        setInBetweenPalette(m_greyFramesLeft, m_greyPaletteAnimation);
        --m_greyFramesLeft;
    }

    // todo: Create minimal set of rectangles to redraw.

    for (int i = 0; i < m_undrawnRects.size(); ++i)
    {
        Rectangle& rect = m_undrawnRects[i];
        int16_t rectHeight = rect.height;
        int16_t rectWidth = rect.width;
        int16_t rectY = rect.y;
        int16_t rectX = rect.x;
        for (int16_t y = 0; y < rectHeight; ++y)
        {
            memcpy(getScreenLine(y + rectY) + rectX, getBackBufferLine(y + rectY) + rectX, rectWidth);
        }
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



void write16bit(uint16_t data, FILE* fp)
{
    fwrite(&data, 2, 1, fp);
}

void write8bit(uint8_t data, FILE* fp)
{
    fwrite(&data, 1, 1, fp);
}

void convertToTga(char far* data, const char* filename)
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

void VgaGfx::saveAsTgaImage(const char* filename)
{
    convertToTga(screen, filename);
}

void VgaGfx::drawDeathEffect()
{
    for (int y = 0; y < SCREEN_H; ++y)
    {
        if ((y % 15) == 0) vsync();
        char *line = getScreenLine(y);

        for (int x = 0; x < SCREEN_W; ++x)
        {
            line[x] = (line[x] + 16);
        }
    }
    setInBetweenPalette(DEATH_ANIMATION_PALETTE_FRAMES - 1, m_greyPaletteAnimation);
    m_greyFramesLeft = DEATH_ANIMATION_PALETTE_FRAMES - 1;
    for (int y = 0; y < SCREEN_H; ++y)
    {
        if ((y % 15) == 0) vsync();
        char *line = getScreenLine(y);

        for (int x = 0; x < SCREEN_W; ++x)
        {
            line[x] = (line[x] - 16);
        }
    }
}

void setFancyWipePalette()
{
    outp(0x3C8, 0);
    for (int i = 0; i < 16; ++i)
    {
        // Colors need to be set in RGB order. As rgbiColors is in BGR order, we need to reverse the order.
        // As VGA only supports 6 bits per color, we need to shift the values by 2 bits.
        outp(0x3C9, rgbiColors[i * 3 + 2] >> 2);
        outp(0x3C9, rgbiColors[i * 3 + 1] >> 2);
        outp(0x3C9, rgbiColors[i * 3 + 0] >> 2);
    }
    for (int i = 0; i < 16; ++i)
    {
        // Colors need to be set in RGB order. As rgbiColors is in BGR order, we need to reverse the order.
        // As VGA only supports 6 bits per color, we need to shift the values by 2 bits.
        outp(0x3C9, rgbiColors[i * 3 + 2] >> 2);
        outp(0x3C9, rgbiColors[i * 3 + 1] >> 2);
        outp(0x3C9, rgbiColors[i * 3 + 0] >> 2);
    }

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
        createFadeToBlackPalette(palette, frameCount, fadeToBlack?(frameCount - frame - 1):frame);
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

#define SCREEN_FADE_FRAMES 5
#define TILES_AT_A_TIME 4

void VgaGfx::fancyWipe(const ImageBase& image)
{
    setFancyWipePalette();

    char* imgData = image.data();
    char* screenData = getScreenLine(0);

    uint16_t rectangleNumber = 0;

    for (uint16_t loop = 0; loop < (200 / TILES_AT_A_TIME); ++loop)
    {

        // first fade out the old image
        for (uint16_t n = 0; n < TILES_AT_A_TIME; ++n)
        {
            Rectangle rect;
            calcNthRectangle(rect, randomShuffle[rectangleNumber + n]);
            for (uint16_t y = 0; y < rect.height; ++y)
            {
                uint16_t offset = (rect.y + y) * SCREEN_W + rect.x;
                for (uint16_t x = 0; x < rect.width; ++x)
                {
                    screenData[offset + x] += 16;
                }
            }
        }
        fadeSecondPalette(SCREEN_FADE_FRAMES, true);

        // now copy the new image
        for (uint16_t n = 0; n < TILES_AT_A_TIME; ++n)
        {
            Rectangle rect;
            calcNthRectangle(rect, randomShuffle[rectangleNumber + n]);
            for (uint16_t y = 0; y < rect.height; ++y)
            {
                uint16_t offset = (rect.y + y) * SCREEN_W + rect.x;
                for (uint16_t x = 0; x < rect.width; ++x)
                {
                    screenData[offset + x] = imgData[offset + x] + 16;
                }
            }
        }
        fadeSecondPalette(SCREEN_FADE_FRAMES, false);

        // now fix colors of copied data
        for (uint16_t n = 0; n < TILES_AT_A_TIME; ++n)
        {
            Rectangle rect;
            calcNthRectangle(rect, randomShuffle[rectangleNumber + n]);
            for (uint16_t y = 0; y < rect.height; ++y)
            {
                uint16_t offset = (rect.y + y) * SCREEN_W + rect.x;
                for (uint16_t x = 0; x < rect.width; ++x)
                {
                    screenData[offset + x] -= 16;
                }
            }
        }

        rectangleNumber += TILES_AT_A_TIME;
    }

    // restore normal palette
    setDefaultVgaPalette();
}