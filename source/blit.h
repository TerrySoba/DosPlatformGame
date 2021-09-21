#ifndef BLIT_H_INCLUDED
#define BLIT_H_INCLUDED

#include "image_base.h"
#include "rectangle.h"

void blit(const ImageBase& source, const Rectangle& sourceRect, const ImageBase& target, const Point& targetPos);


#endif