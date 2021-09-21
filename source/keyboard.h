#ifndef KEYBOARD_H_INCLUDED
#define KEYBOARD_H_INCLUDED

#include <stdint.h>

volatile extern uint8_t s_scancode;

extern volatile uint8_t s_keyLeft;
extern volatile uint8_t s_keyRight;
extern volatile uint8_t s_keyUp;
extern volatile uint8_t s_keyDown;

extern volatile uint8_t s_keyCtrl;
extern volatile uint8_t s_keyAlt;
extern volatile uint8_t s_keySpace;
extern volatile uint8_t s_keyEsc;

class Keyboard
{
public:
    Keyboard();
    ~Keyboard();
private:
    void __interrupt __far (*m_oldInterrupt)();
};


#endif
