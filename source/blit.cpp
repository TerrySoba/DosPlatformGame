#include "blit.h"
#include <string.h>


void memcpyTransparent(void* dest, const void* src, size_t count, uint8_t transparentColor)
{
    char* d = (char*)dest;
    const char* s = (const char*)src;
    for (size_t i = 0; i < count; ++i)
    {
        if (s[i] != transparentColor)
        {
            d[i] = s[i];
        }
    }
}


void blit(
    const void* source, uint16_t sourceWidth, uint16_t sourceHeight,
    const Rectangle& sourceRect,
    void* target, uint16_t targetWidth, uint16_t targetHeight,
    const Point& targetPos)
{
    Rectangle src = Rectangle(0,0,sourceWidth, sourceHeight).intersection(sourceRect);
    Rectangle dest = Rectangle(0,0,targetWidth, targetHeight)
                     .intersection(Rectangle(src - Point(sourceRect.x, sourceRect.y) + targetPos));
    src = src.intersection(dest - targetPos + Point(sourceRect.x, sourceRect.y));

    const int16_t& copyWidth = dest.width;
    const int16_t& copyHeight = dest.height;
    const int16_t& destY = dest.y;
    const int16_t& destX = dest.x;
    const int16_t& srcX = src.x;
    const int16_t& srcY = src.y;

    for (int16_t y = 0; y < copyHeight; ++y)
    {
        memcpy(
            (uint8_t*)target + (destY + y) * targetWidth + destX,
            (uint8_t*)source + (srcY + y) * sourceWidth + srcX,
            copyWidth);
    }
}


void blit(const ImageBase& source, const Rectangle& sourceRect, const ImageBase& target, const Point& targetPos)
{
    blit(source.data(), source.width(), source.height(), sourceRect,
         target.data(), target.width(), target.height(), targetPos);
}


void blitTransparent(
    const void* source, uint16_t sourceWidth, uint16_t sourceHeight,
    const Rectangle& sourceRect,
    void* target, uint16_t targetWidth, uint16_t targetHeight,
    const Point& targetPos,
    uint8_t transparentColor)
{
    Rectangle src = Rectangle(0,0,sourceWidth, sourceHeight).intersection(sourceRect);
    Rectangle dest = Rectangle(0,0,targetWidth, targetHeight)
                     .intersection(Rectangle(src - Point(sourceRect.x, sourceRect.y) + targetPos));
    src = src.intersection(dest - targetPos + Point(sourceRect.x, sourceRect.y));

    const int16_t& copyWidth = dest.width;
    const int16_t& copyHeight = dest.height;
    const int16_t& destY = dest.y;
    const int16_t& destX = dest.x;
    const int16_t& srcX = src.x;
    const int16_t& srcY = src.y;

    for (int16_t y = 0; y < copyHeight; ++y)
    {
        memcpyTransparent(
            (uint8_t*)target + (destY + y) * targetWidth + destX,
            (uint8_t*)source + (srcY + y) * sourceWidth + srcX,
            copyWidth, transparentColor);
    }
}

void blitTransparent(
    const ImageBase& source,
    const Rectangle& sourceRect,
    const ImageBase& target,
    const Point& targetPos,
    uint8_t transparentColor)
{
    blitTransparent(
        source.data(), source.width(), source.height(), sourceRect,
        target.data(), target.width(), target.height(), targetPos,
        transparentColor);
}