#ifndef BIOS_FUNCTIONS_H
#define BIOS_FUNCTIONS_H

#include <stdint.h>

void printCh(char ch);
#pragma aux printCh = \
    "mov ah, 02h"   \
    "int 0x21"      \
    parm    [dl]    \
    modify  [ax];

extern void disableCursor();
#pragma aux disableCursor =    \
    "mov ah, 01h"              \
    "mov ch, 3Fh"              \
    "int 10h"                  \
    modify [ah ch];

extern void setCursor(uint8_t row, uint8_t col);
#pragma aux setCursor =   \
        "mov ah, 02h" \
		"mov bh, 00h" \
		"int 10h" \
        modify[ah bh dx] \
        parm [dh] [dl];

#endif // BIOS_FUNCTIONS_H