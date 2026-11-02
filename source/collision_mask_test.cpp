#include "unit_test.h"
#include "collision_mask.h"

// 4x4 mask with a single opaque pixel at (1, 2)
static const uint16_t s_dotRows[] = {0x0000, 0x0000, 0x4000, 0x0000};

// 16x2 mask, first row fully opaque, second row only the rightmost pixel
static const uint16_t s_wideRows[] = {0xFFFF, 0x0001};

static CollisionMask makeMask(int16_t width, int16_t height, const uint16_t* rows)
{
    CollisionMask mask;
    mask.width = width;
    mask.height = height;
    mask.rows = rows;
    return mask;
}

TEST(CollisionMaskSamePosition)
{
    CollisionMask dot = makeMask(4, 4, s_dotRows);
    ASSERT_TRUE(masksOverlap(dot, 10, 10, dot, 10, 10));
    ASSERT_FALSE(masksOverlap(dot, 10, 10, dot, 11, 10));
    ASSERT_FALSE(masksOverlap(dot, 10, 10, dot, 10, 11));
}

TEST(CollisionMaskOffsets)
{
    CollisionMask dot = makeMask(4, 4, s_dotRows);
    CollisionMask wide = makeMask(16, 2, s_wideRows);

    // the dot is at pixel (x + 1, y + 2), the full row of wide covers (x .. x + 15, y)
    ASSERT_TRUE(masksOverlap(wide, 0, 2, dot, 0, 0));    // dot at (1, 2)
    ASSERT_TRUE(masksOverlap(dot, 0, 0, wide, 0, 2));    // order does not matter
    ASSERT_TRUE(masksOverlap(wide, 0, 2, dot, 14, 0));   // dot at (15, 2), last pixel of the row
    ASSERT_FALSE(masksOverlap(wide, 0, 2, dot, 15, 0));  // dot at (16, 2), right of the row
    ASSERT_FALSE(masksOverlap(wide, 0, 2, dot, -2, 0));  // dot at (-1, 2), left of the row
    ASSERT_TRUE(masksOverlap(dot, 14, 0, wide, 0, 2));   // negative offset of wide relative to dot, dot at (15, 2)
    ASSERT_FALSE(masksOverlap(dot, 15, 0, wide, 0, 2));  // dot at (16, 2)

    // second row of wide only has its rightmost pixel (15, 3)
    ASSERT_TRUE(masksOverlap(wide, 0, 2, dot, 14, 1));   // dot at (15, 3)
    ASSERT_FALSE(masksOverlap(wide, 0, 2, dot, 13, 1));  // dot at (14, 3)

    // no overlapping rows
    ASSERT_FALSE(masksOverlap(wide, 0, 2, dot, 0, 2));   // dot at (1, 4), below wide
    ASSERT_FALSE(masksOverlap(wide, 0, 2, dot, 0, -3));  // dot at (1, -1), above wide

    // far away
    ASSERT_FALSE(masksOverlap(wide, 0, 0, dot, 100, 0));
    ASSERT_FALSE(masksOverlap(wide, 100, 0, dot, 0, 0));
}

TEST(CollisionMaskWithoutRows)
{
    // a missing mask means the rectangle test alone decides
    CollisionMask dot = makeMask(4, 4, s_dotRows);
    CollisionMask none = makeMask(32, 32, 0);
    ASSERT_TRUE(masksOverlap(dot, 0, 0, none, 0, 0));
    ASSERT_TRUE(masksOverlap(none, 0, 0, dot, 0, 0));
}
