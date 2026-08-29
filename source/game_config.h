#ifndef GAME_CONFIG_H
#define GAME_CONFIG_H

#include <stdint.h>

struct GameConfig
{
    // sound section
    bool enableSound;
    bool enableMusic;

    // keyboard section
    uint8_t keyUp;
    uint8_t keyDown;
    uint8_t keyLeft;
    uint8_t keyRight;
    uint8_t keyJump;
    uint8_t keyAction;

    // joystick section
    uint8_t joyJump;
    uint8_t joyAction;
};

GameConfig parseGameConfig(const char* fileName);
void setIniValue(const char* fileName, const char* section, const char* key, const char* value);


#endif // GAME_CONFIG_H