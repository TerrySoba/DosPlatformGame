#ifndef _PHYSICS_EVENT_INCLUDED_H
#define _PHYSICS_EVENT_INCLUDED_H

#include "rectangle.h"

enum LevelTransition
{
    RIGHT  = 1,
    LEFT   = 2,
    BOTTOM = 3,
    TOP    = 4
};


class PhysicsCallback
{
public:
    virtual void levelTransition(LevelTransition transition) = 0;
    virtual void collectApple(Point point) = 0;
    virtual void collectJetPack(Point point) = 0;
    virtual void collectSunItem(Point point) = 0;
    virtual void touchButton(uint16_t id, ButtonType type) = 0;
    virtual void onDeath() = 0;
    virtual void onCreditsWarp() = 0;
};

#endif
