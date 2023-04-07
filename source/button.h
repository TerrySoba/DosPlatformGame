#ifndef BUTTON_H
#define BUTTON_H

#include "rectangle.h"

enum ButtonType
{
    BUTTON_ON = 1,  // Turns the button on when touched
    BUTTON_OFF = 2, // Turns the button off when touched
};

struct Button : public Rectangle
{
    Button() {}

    Button(uint16_t id, ButtonType type, Rectangle rect) : buttonId(id), buttonType(type) {
        *((Rectangle*)this) = rect;
    }
    uint16_t buttonId;       // id of button
    ButtonType buttonType;   // type of button
};

#endif