#include "key_mapper.h"

#ifdef PLATFORM_DOS
#include "platform/dos/keyboard_dos.h"
#include "platform/dos/joystick_dos.h"
#endif

#ifdef PLATFORM_SDL
#include "platform/sdl/keyboard_sdl.h"
#include "platform/sdl/joystick_sdl.h"
#endif

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
    if (s_keyAlt || joystick & JOY_BUTTON_1)
    {
        keys |= KEY_JUMP;
    }
    if (s_keyCtrl || joystick & JOY_BUTTON_2)
    {
        keys |= KEY_ACTION1;
    }
    
    return keys;
}
