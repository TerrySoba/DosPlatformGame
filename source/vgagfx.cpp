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


VgaGfx::VgaGfx()
{
    long int screenBytes = SCREEN_W * SCREEN_H;

    m_backgroundImage = new char[screenBytes];
    m_screenBuffer = new char[screenBytes];
    
    memset(m_backgroundImage, 0xff, screenBytes);
    memset(m_screenBuffer, 0x0, screenBytes);

    Rectangle rect(0, 0, SCREEN_W, SCREEN_H);
    m_dirtyRects.push_back(rect);

    videoInit(0x13);
}

VgaGfx::~VgaGfx()
{
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

#define lower(x) (x & 0x0f)
#define upper(x) ((x & 0xf0) >> 4)

#define set_lower(x, val) ((x & 0xf0) | val)
#define set_upper(x, val) ((x & 0x0f) | (val << 4))

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


static const uint8_t rgbiColors[] = {
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
    0xFF,0x55,0xFF,
    0xFF,0x55,0xFF,
    0x55,0xFF,0xFF,
    0xFF,0xFF,0xFF,
};

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
