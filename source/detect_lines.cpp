#include "detect_lines.h"


class MapData
{
public:
    MapData(const uint8_t* data, int dataWidth, int dataHeight, bool swap) :
        m_data(data), m_width(dataWidth), m_height(dataHeight), m_swap(swap)
    {
    }

    uint8_t get(int x, int y)
    {
        return m_swap ? 
                    m_data[m_width * x + y]:
                    m_data[m_width * y + x];
    }

    int width()
    {
        return m_swap ? 
                    m_height :
                    m_width;
    }

    int height()
    {
        return m_swap ? 
                    m_width :
                    m_height;
    }

private:
    const uint8_t* m_data;
    int m_width;
    int m_height;
    bool m_swap;
};


tnd::vector<Rectangle> detectLines(const uint8_t data[], int dataWidth, int dataHeight, Direction direction, uint8_t lineIndex)
{
    tnd::vector<Rectangle> rectangles;

    MapData map(data, dataWidth, dataHeight, direction == VERTICAL);

    for (int y = 0; y < map.height(); ++y)
    {
        int lineStart = -1;
        for (int x = 0; x < map.width(); ++x)
        {
            if (map.get(x,y) == lineIndex)
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
            rectangles.push_back(Rectangle(lineStart, y, map.width() - lineStart, 1));
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