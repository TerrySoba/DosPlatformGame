#include "post_codes.h"

// #include <dos.h>
#include <conio.h>

void writePostCode(uint8_t code)
{
    outp(0x80, code);
}