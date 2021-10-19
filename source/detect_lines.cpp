#include "detect_lines.h"


typedef uint8_t (*CsvGetterPtr)(CsvReader<uint8_t>& reader, int x, int y);

uint8_t getCsvEntry(CsvReader<uint8_t>& reader, int x, int y)
{
    return reader.get(x,y);
}

uint8_t getCsvEntrySwapped(CsvReader<uint8_t>& reader, int x, int y)
{
    return reader.get(y,x);
}

tnd::vector<Rectangle> detectLines(CsvReader<uint8_t>& reader, Direction direction, uint8_t lineIndex)
{
    tnd::vector<Rectangle> rectangles;

    int width;
    int height;
    CsvGetterPtr get;

    if (direction == HORIZONTAL)
    {
        width = reader.width();
        height = reader.height();
        get = getCsvEntry;
    } else {
        width = reader.height();
        height = reader.width();
        get = getCsvEntrySwapped;
    }

    for (int y = 0; y < height; ++y)
    {
        int lineStart = -1;
        for (int x = 0; x < width; ++x)
        {
            if (get(reader,x,y) == lineIndex)
            {
                if (lineStart == -1) lineStart = x;
            }
            else
            {
                if (lineStart != -1)
                {
                    rectangles.push_back(Rectangle(lineStart, y, x - lineStart, 1));
                    lineStart = -1;
                }
            }
        }
        if (lineStart != -1)
        {
            rectangles.push_back(Rectangle(lineStart, y, width - lineStart, 1));
            lineStart = -1;
        }
    }

    // if vertical swap x and y, width and height
    if (direction == VERTICAL)
    {
        for (int i = 0; i < rectangles.size(); ++i)
        {
            Rectangle& r = rectangles[i];
            rectangles[i] = Rectangle(r.y, r.x, r.height, r.width);
        }
    }

    return rectangles;
}


/**
 * This functions searches for a rectangle in the given data.
 * 
 * NOTE:
 *   This function makes the assumption that there actually is one rectangle or
 *   no rectangle in the given data. If there are mutiple rectangles in the
 *   data or if there are shapes that are not rectangles it will produce
 *   incorrect results.
 */
tnd::vector<Rectangle> findSingleRectangle(const uint8_t data[], int dataWidth, int dataHeight, uint8_t rectangleIndex)
{
    tnd::vector<Rectangle> ret;

    if (!data) {
        return ret;
    }

    int firstX = -1;
    int firstY = -1;
    int lastX = -1;
    int lastY = -1;

    for (int y = 0; y < dataHeight; ++y)
    {
        for (int x = 0; x < dataWidth; ++x)
        {
            if (data[y * dataWidth + x] == rectangleIndex)
            {
                if (firstX == -1 && firstY == -1)
                {
                    firstX = x;
                    firstY = y;
                }
                lastX = x;
                lastY = y;
            }
        }
    }

    if ((firstX != -1) && 
        (firstY != -1) &&
        (lastX != -1) && 
        (lastY != -1))
    {
        ret.push_back(Rectangle(firstX, firstY, lastX - firstX + 1, lastY - firstY + 1));
    }

    return ret;
}