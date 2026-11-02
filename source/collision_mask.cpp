#include "collision_mask.h"

bool masksOverlap(const CollisionMask& a, int16_t ax, int16_t ay,
                  const CollisionMask& b, int16_t bx, int16_t by)
{
    if (!a.rows || !b.rows)
    {
        return true;
    }

    // position of b relative to a
    int16_t dx = bx - ax;
    if (dx >= COLLISION_MASK_MAX_WIDTH || dx <= -COLLISION_MASK_MAX_WIDTH)
    {
        return false;
    }

    // rows of a that are covered by b
    int16_t firstRow = by - ay;
    if (firstRow < 0) firstRow = 0;
    int16_t endRow = by + b.height - ay;
    if (endRow > a.height) endRow = a.height;

    for (int16_t y = firstRow; y < endRow; ++y)
    {
        // move the row of b to the x position of a
        uint16_t rowB = b.rows[y + ay - by];
        if (dx >= 0)
        {
            rowB >>= dx;
        }
        else
        {
            rowB = (uint16_t)(rowB << -dx);
        }

        if (a.rows[y] & rowB)
        {
            return true;
        }
    }

    return false;
}
