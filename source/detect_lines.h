#ifndef _DETECT_LINES_H_INCLUDED
#define _DETECT_LINES_H_INCLUDED

#include "csv_reader.h"
#include "rectangle.h"
#include "vector.h"

enum Direction
{
    HORIZONTAL = 1,
    VERTICAL = 2,
};

tnd::vector<Rectangle> detectLines(CsvReader<uint8_t>& reader, Direction direction, uint8_t lineIndex);
tnd::vector<Rectangle> findSingleRectangle(const uint8_t data[], int dataWidth, int dataHeight, uint8_t rectangleIndex);

#endif