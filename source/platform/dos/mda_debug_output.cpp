#include "mda_debug_output.h"

#include <dos.h>
#include <string.h>
#include <stdint.h>

#define MDA_VIDEO_SEG 0xB000

// Define a far pointer to the MDA video memory
unsigned char far *mda_vram = (unsigned char far *)MK_FP(MDA_VIDEO_SEG, 0x0000);

void putMdaChar(int col, int row, char character, char attribute) {
    // Calculate the offset: (row * 80 + col) * 2 bytes/character
    long offset = (long)row * 80 * 2 + (long)col * 2;
    
    // Write character and attribute
    *(mda_vram + offset) = character;
    *(mda_vram + offset + 1) = attribute;
}

void clearMdaScreen() {
    uint16_t far *mda_vram_w = (uint16_t far *)mda_vram;
    for (int i = 0; i < 25 * 80; ++i) {
        mda_vram_w[i] = 0x0720;
    }
}

void mdaPrint(const char* message, int x, int y, char attribute) {
    int col = x;
    int row = y;
    while (*message) {
        putMdaChar(col++, row, *message++, attribute);
        if (col >= 80) { // Move to next line if end of line is reached
            col = 0;
            row++;
            if (row >= 25) {
                break; // Stop if we exceed screen height
            }
        }
    }
}
