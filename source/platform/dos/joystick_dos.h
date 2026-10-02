#ifndef _INCLUDE_DOS_JOYSTICK_H
#define _INCLUDE_DOS_JOYSTICK_H

#include <stdint.h>

enum JoystickState
{
    JOY_LEFT = 1,
    JOY_RIGHT = 2,
    JOY_UP = 4,
    JOY_DOWN = 8,
    JOY_BUTTON_1 = 16,
    JOY_BUTTON_2 = 32,
    JOY_BUTTON_3 = 64,
    JOY_BUTTON_4 = 128
};

void calibrateJoystick();
uint8_t readJoystick();

// reads only the joystick buttons (JOY_BUTTON_x bits), does not require calibration
uint8_t readJoystickButtons();

#endif
