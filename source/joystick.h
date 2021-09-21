#ifndef _INCLUDE_JOYSTICK_H
#define _INCLUDE_JOYSTICK_H

#include <stdint.h>

enum JoystickState
{
    JOY_LEFT = 1,
    JOY_RIGHT = 2,
    JOY_UP = 4,
    JOY_DOWN = 8,
    JOY_BUTTON_1 = 16,
    JOY_BUTTON_2 = 32
};

void calibrateJoystick();
uint8_t readJoystick();

#endif
