#ifndef _DETECT_LINES_H_INCLUDED
#define _DETECT_LINES_H_INCLUDED

#include "rectangle.h"
#include "vector.h"

enum Direction
{
    HORIZONTAL = 1,
    VERTICAL = 2,
};

tnd::vector<Rectangle> detectLines(const uint8_t data[], int dataWidth, int dataHeight, Direction direction, uint8_t lineIndex);
tnd::vector<Rectangle> findSingleRectangle(const uint8_t data[], int dataWidth, int dataHeight, uint8_t rectangleIndex);

#endif