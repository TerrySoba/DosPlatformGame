#ifndef GAME_CONFIG_H
#define GAME_CONFIG_H

#include <stdint.h>

#define DEFAULT_CONFIG_NAME "config.ini"

struct KeyboardConfig
{
    uint8_t keyUp;
    uint8_t keyDown;
    uint8_t keyLeft;
    uint8_t keyRight;
    uint8_t keyJump;
    uint8_t keyAction;
};

struct JoystickConfig
{
    uint8_t joyJump;
    uint8_t joyAction;
};

struct GameConfig
{
    // sound section
    bool enableSound;
    bool enableMusic;

    // input section
    KeyboardConfig keyboard;
    JoystickConfig joystick;
};

GameConfig parseGameConfig(const char* fileName);
void setIniValue(const char* fileName, const char* section, const char* key, const char* value);
void writeKeyboardConfig(const char* fileName, const KeyboardConfig& keyboard);

#endif // GAME_CONFIG_H