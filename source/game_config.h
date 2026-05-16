#ifndef GAME_CONFIG_H
#define GAME_CONFIG_H

#include <stdint.h>

struct GameConfig
{
    // Keyboard keys
    uint8_t upKey;
    uint8_t downKey;
    uint8_t leftKey;
    uint8_t rightKey;
    uint8_t action1Key;
    uint8_t action2Key;

    // Joystick key map
    uint8_t action1JoyButton;
    uint8_t action2JoyButton;
};

GameConfig readGameConfig(const char* filename);

#endif