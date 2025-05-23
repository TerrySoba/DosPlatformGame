#ifndef BLIT_H_INCLUDED
#define BLIT_H_INCLUDED

#include "image_base.h"
#include "rectangle.h"

void blit(
    const uint8_t* source, uint16_t sourceWidth, uint16_t sourceHeight,
    const Rectangle& sourceRect,
    uint8_t* target, uint16_t targetWidth, uint16_t targetHeight,
    const Point& targetPos);

void blit(
    const ImageBase& source,
    const Rectangle& sourceRect,
    const ImageBase& target,
    const Point& targetPos);

void blitTransparent(
    const uint8_t* source, uint16_t sourceWidth, uint16_t sourceHeight,
    const Rectangle& sourceRect,
    uint8_t* target, uint16_t targetWidth, uint16_t targetHeight,
    const Point& targetPos,
    uint8_t transparentColor);

void blitTransparent(
    const ImageBase& source,
    const Rectangle& sourceRect,
    const ImageBase& target,
    const Point& targetPos,
    uint8_t transparentColor);

#endif