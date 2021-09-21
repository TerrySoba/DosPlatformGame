#include "keyboard.h"

#include <dos.h>
#include <stdint.h>

volatile uint8_t s_scancode;

volatile uint8_t s_keyLeft;
volatile uint8_t s_keyRight;
volatile uint8_t s_keyUp;
volatile uint8_t s_keyDown;

volatile uint8_t s_keyCtrl;
volatile uint8_t s_keyAlt;
volatile uint8_t s_keySpace;
volatile uint8_t s_keyEsc;


#define LEFT_KEY  0x4b
#define RIGHT_KEY 0x4d
#define UP_KEY    0x48
#define DOWN_KEY  0x50
#define CTRL_KEY  0x1d
#define ALT_KEY   0x38
#define SPACE_KEY 0x39
#define ESC_KEY   0x01

#define SCANCODE_MASK 127
#define PRESS_MASK 128

void __interrupt __far handleScancode( void )
{
    uint8_t code;
    __asm   {
        cli
        in    al, 060h       /* read scan code */
        mov   code, al
        in    al, 061h       /* read keyboard status */
        mov   bl, al
        or    al, 080h
        out   061h, al       /* set bit 7 and write */
        mov   al, bl
        out   061h, al       /* write again, bit 7 clear */

        mov   al, 020h       /* reset PIC */
        out   020h, al

        /* end of re-set code */

        sti
    }

    s_scancode = code;

    switch (code & SCANCODE_MASK)
    {
        case LEFT_KEY:
            s_keyLeft = !(code & PRESS_MASK);
            break;
        case RIGHT_KEY:
            s_keyRight = !(code & PRESS_MASK);
            break;
        case UP_KEY:
            s_keyUp = !(code & PRESS_MASK);
            break;
        case DOWN_KEY:
            s_keyDown = !(code & PRESS_MASK);
            break;
        case CTRL_KEY:
            s_keyCtrl = !(code & PRESS_MASK);
            break;
        case ALT_KEY:
            s_keyAlt = !(code & PRESS_MASK);
            break;
        case SPACE_KEY:
            s_keySpace = !(code & PRESS_MASK);
            break;
        case ESC_KEY:
            s_keyEsc = !(code & PRESS_MASK);
            break;
    }

}

#define KEYBOARD_INTERRUPT 9

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

    m_oldInterrupt = _dos_getvect(KEYBOARD_INTERRUPT);
    _dos_setvect(KEYBOARD_INTERRUPT, handleScancode);
}

Keyboard::~Keyboard()
{
    _dos_setvect(KEYBOARD_INTERRUPT, m_oldInterrupt);
}
