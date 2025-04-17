#include "keyboard_sdl.h"

volatile uint8_t s_scancode;

volatile uint8_t s_keyLeft;
volatile uint8_t s_keyRight;
volatile uint8_t s_keyUp;
volatile uint8_t s_keyDown;
volatile uint8_t s_keyCtrl;
volatile uint8_t s_keyAlt;
volatile uint8_t s_keySpace;
volatile uint8_t s_keyEsc;


Keyboard::Keyboard()
{
    s_keyLeft = 0;
    s_keyRight = 0;
    s_keyUp = 0;
    s_keyDown = 0;
    s_keyCtrl = 0;
    s_keyAlt = 0;
    s_keySpace = 0;
    s_keyEsc = 0;
}

Keyboard::~Keyboard()
{

}

