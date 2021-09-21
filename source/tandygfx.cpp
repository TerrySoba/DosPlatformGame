#include "tandygfx.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dos.h>
#include <conio.h>
#include <stdio.h>

#define inport(px) inpw(px)
#define inportb(px) inp(px)

#define TANDY_SCREEN_W 320L
#define TANDY_SCREEN_H 200L


extern void videoInit(void);
#pragma aux videoInit =    \
    "mov ah, 00h"          \
    "mov al, 09h"          \
    "int 10h"              \
    ;


// static void videoInit()
// {
//     __asm{
//         mov ah, 00h
//         mov al, 09h
//         int 10h
//        }
// }

int compareRectangles(const void* a, const void* b) {
   return ( ((Rectangle*)a)->y > ((Rectangle*)b)->y );
}


void sortRects(std::vector<Rectangle>& rects)
{
    qsort(&rects[0], rects.size(), sizeof(Rectangle), compareRectangles);
}


TandyGfx::TandyGfx()
{
    long int screenBytes = TANDY_SCREEN_W * TANDY_SCREEN_H / 2;

    m_backgroundImage = new char[screenBytes];
    m_screenBuffer = new char[screenBytes];
    
    memset(m_backgroundImage, 0xff, screenBytes);
    memset(m_screenBuffer, 0x0, screenBytes);

    Rectangle rect = {0,0,TANDY_SCREEN_W,TANDY_SCREEN_H};
    m_dirtyRects.push_back(rect);

    videoInit();
}

TandyGfx::~TandyGfx()
{
    delete[] m_screenBuffer;
    delete[] m_backgroundImage;
}

#define CGA_STATUS_REG 0x03DA
#define VERTICAL_RETRACE_BIT 0x08

#define waitForVsync() \
    while (inportb(CGA_STATUS_REG) & VERTICAL_RETRACE_BIT); \
    while (!(inportb(CGA_STATUS_REG) & VERTICAL_RETRACE_BIT))

#define LINE_BYTES 160L

static char far* screen[4] = {(char far*)(0xB8000000L), (char far*)(0xB8002000L), (char far*)(0xB8004000L), (char far*)(0xB8006000L)};

static char far* TandyGfx::getScreenLine(int line)
{
    return screen[line & 3] + LINE_BYTES * (line >> 2);
}

char far* TandyGfx::getBackBufferLine(int line)
{
    return m_screenBuffer + LINE_BYTES * line;
}

char far* TandyGfx::getBackgroundImageLine(int line)
{
    return m_backgroundImage + LINE_BYTES * line;
}


void TandyGfx::vsync()
{
    waitForVsync();
}

#define lower(x) (x & 0x0f)
#define upper(x) ((x & 0xf0) >> 4)

#define set_lower(x, val) ((x & 0xf0) | val)
#define set_upper(x, val) ((x & 0x0f) | (val << 4))


void TandyGfx::drawImage(const ImageBase& image, int targetX, int targetY)
{
    const char* imageData = image.data();
    const int imageLineBytes = image.width() / 2;

    Rectangle rect = {
        targetX, targetY,
        image.width(), image.height()};
    m_dirtyRects.push_back(rect);
    m_undrawnRects.push_back(rect);

    for (int y = 0; y < image.height(); ++y)
    {
        // memcpy(m_screenBuffer + LINE_BYTES * (targetY + y) + targetX / 2, imageData + imageLineBytes * y, imageLineBytes);
        memcpy(getBackBufferLine(y + targetY) + targetX / 2, imageData + imageLineBytes * y, imageLineBytes);
    }
}

void TandyGfx::clear()
{
    for (int i = 0; i < m_dirtyRects.size(); ++i)
    {
        m_undrawnRects.push_back(m_dirtyRects[i]);
        const Rectangle& rect = m_dirtyRects[i];
        for (int y = 0; y < rect.height; ++y)
        {
            // memcpy(m_screenBuffer + LINE_BYTES * (targetY + y) + targetX / 2, imageData + imageLineBytes * y, imageLineBytes);
            // memset(getScreenLine(y + rect.y) + rect.x / 2, 0, rect.width / 2);
            // memset(getBackBufferLine(y + rect.y) + rect.x / 2, 0, rect.width / 2);
            memcpy(getBackBufferLine(y + rect.y) + rect.x / 2, getBackgroundImageLine(y + rect.y) + rect.x / 2, rect.width / 2);
        }
    }
    m_dirtyRects.clear();
}

void TandyGfx::drawScreen()
{   
    vsync();

    // todo: Create minimal set of rectangles to redraw.

    for (int i = 0; i < m_undrawnRects.size(); ++i)
    {
        Rectangle& rect = m_undrawnRects[i];
        for (int y = 0; y < rect.height; ++y)
        {
            // memcpy(m_screenBuffer + LINE_BYTES * (targetY + y) + targetX / 2, imageData + imageLineBytes * y, imageLineBytes);
            memcpy(getScreenLine(y + rect.y) + rect.x / 2, getBackBufferLine(y + rect.y) + rect.x / 2, rect.width / 2);
        }
    }

    m_undrawnRects.clear();
}


void TandyGfx::setBackground(const ImageBase& image)
{
    if (image.width() == TANDY_SCREEN_W &&
        image.height() == TANDY_SCREEN_H)
    {
        memcpy(m_backgroundImage, image.data(), TANDY_SCREEN_W * TANDY_SCREEN_H / 2);
        // memcpy(m_screenBuffer, image.data(), image.width() * image.height() / 2);
        Rectangle rect = {
            0, 0,
            image.width(), image.height()};
        m_dirtyRects.push_back(rect);
    }
}
