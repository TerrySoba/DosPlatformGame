#ifndef _COLLISION_MASK_H_INCLUDED
#define _COLLISION_MASK_H_INCLUDED

#include <stdint.h>

// Frames wider than this get no mask, a row is stored in one uint16_t.
#define COLLISION_MASK_MAX_WIDTH 16

/**
 * Marks the opaque pixels of a sprite frame, one bit per pixel.
 * Bit 15 of a row is the leftmost pixel.
 */
struct CollisionMask
{
    int16_t width;
    int16_t height;
    const uint16_t* rows; // height entries, 0 if there is no mask for this frame
};

/**
 * Returns true if an opaque pixel of mask a at pixel position (ax, ay) covers an opaque pixel
 * of mask b at (bx, by).
 *
 * If one of the masks has no rows, true is returned, so that the caller falls back to the
 * result of its rectangle test.
 */
bool masksOverlap(const CollisionMask& a, int16_t ax, int16_t ay,
                  const CollisionMask& b, int16_t bx, int16_t by);

#endif
