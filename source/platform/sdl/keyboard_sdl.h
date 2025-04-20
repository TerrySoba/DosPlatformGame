#ifndef KEYBOARD_SDL_H_INCLUDED
#define KEYBOARD_SDL_H_INCLUDED

#include <stdint.h>

#include "keyboard.h"

class KeyboardSdl
{
public:
    void keyDown(uint32_t keyCode);
    void keyUp(uint32_t keyCode);
};


#endif
