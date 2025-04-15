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

}

Keyboard::~Keyboard()
{

}

