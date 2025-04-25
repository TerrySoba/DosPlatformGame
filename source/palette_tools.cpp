#include "palette_tools.h"

// convert RGB to grayscale according to rec601 luma
uint8_t rgbToGray(uint8_t r, uint8_t g, uint8_t b)
{
    // return 0.299 * r + 0.587 * g + 0.114 * b; // rec601 luma
    return 0.133 * r + 0.433 * g + 0.433 * b; // more artistic luma
}
