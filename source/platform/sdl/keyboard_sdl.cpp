#include "keyboard_sdl.h"

#include <SDL3/SDL.h>
#include <map>

static const std::map<SDL_Keycode, volatile uint8_t *> keyMap{
    {SDLK_UP, &s_keyUp},
    {SDLK_DOWN, &s_keyDown},
    {SDLK_LEFT, &s_keyLeft},
    {SDLK_RIGHT, &s_keyRight},
    {SDLK_LCTRL, &s_keyCtrl},
    {SDLK_LALT, &s_keyAlt},
    {SDLK_SPACE, &s_keySpace},
    {SDLK_ESCAPE, &s_keyEsc},
};

void KeyboardSdl::keyDown(uint32_t keyCode)
{
    auto it = keyMap.find(static_cast<SDL_Keycode>(keyCode));
    if (it != keyMap.end()) {
        *(it->second) = true;
    }
}

void KeyboardSdl::keyUp(uint32_t keyCode)
{
    auto it = keyMap.find(static_cast<SDL_Keycode>(keyCode));
    if (it != keyMap.end()) {
        *(it->second) = false;
    }
}
