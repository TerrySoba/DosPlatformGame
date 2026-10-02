#include "key_mapper.h"

#ifdef PLATFORM_DOS
#include "platform/dos/keyboard_dos.h"
#include "platform/dos/joystick_dos.h"
#endif

#ifdef PLATFORM_SDL
#include "platform/sdl/keyboard_sdl.h"
#include "platform/sdl/joystick_sdl.h"
#endif

static uint8_t s_joyJumpMask = JOY_BUTTON_1;
static uint8_t s_joyActionMask = JOY_BUTTON_2;

void KeyMapper::setJoystickButtons(uint8_t jump, uint8_t action)
{
    s_joyJumpMask = (uint8_t)(JOY_BUTTON_1 << (jump & 3));
    s_joyActionMask = (uint8_t)(JOY_BUTTON_1 << (action & 3));
}

KeyBits KeyMapper::getKeys() const
{
    uint8_t joystick = readJoystick();
    KeyBits keys = 0;

    if (s_keyDown || joystick & JOY_DOWN)
    {
        keys |= KEY_DOWN;
    }
    if (s_keyUp || joystick & JOY_UP)
    {
        keys |= KEY_UP;
    }
    if (s_keyLeft || joystick & JOY_LEFT)
    {
        keys |= KEY_LEFT;
    }
    if (s_keyRight || joystick & JOY_RIGHT)
    {
        keys |= KEY_RIGHT;
    }
    if (s_keyAlt || joystick & s_joyJumpMask)
    {
        keys |= KEY_JUMP;
    }
    if (s_keyCtrl || joystick & s_joyActionMask)
    {
        keys |= KEY_ACTION1;
    }
    
    return keys;
}
