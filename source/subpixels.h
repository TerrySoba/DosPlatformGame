#ifndef SUBPIXELS_H_INCLUDED
#define SUBPIXELS_H_INCLUDED

// Physics calculations are done on subpixels.
// One pixel constitutes 16 subpixels, so shifting by 4
// is used to transform between pixels and subpixels.

#define PIXEL_TO_SUBPIXEL(x) ((x) << 4)
#define SUBPIXEL_TO_PIXEL(x) ((x) >> 4)

#endif