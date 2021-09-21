#include "blit.h"
#include <string.h>


void blit(const ImageBase& source, const Rectangle& sourceRect, const ImageBase& target, const Point& targetPos)
{
    Rectangle src = Rectangle(0,0,source.width(), source.height()).intersection(sourceRect);
    Rectangle dest = Rectangle(0,0,target.width(), target.height())
                     .intersection(Rectangle(src - Point(sourceRect.x, sourceRect.y) + targetPos));
    src = src.intersection(dest - targetPos + Point(sourceRect.x, sourceRect.y));

    int16_t copyWidth = dest.width;
    int16_t copyHeight = dest.height;
    int16_t destY = dest.y;
    int16_t destX = dest.x;
    int16_t srcX = src.x;
    int16_t srcY = src.y;

    for (int16_t y = 0; y < copyHeight; ++y)
    {
        memcpy(
            target.linePtr(destY + y) + destX,
            source.linePtr(srcY + y) + srcX,
            copyWidth);
    }
}
