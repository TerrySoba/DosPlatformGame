#ifndef _FIRE_BALL_H_INCLUDED
#define _FIRE_BALL_H_INCLUDED

#include "rectangle.h"
#include "animation.h"
#include "vector.h"

class FireBall
{
public:
    FireBall(Rectangle collision, tnd::shared_ptr<Animation> animation);
    ~FireBall();

    void walk();

    Rectangle getPos();

    void reset();

private:
    const Rectangle m_collision;
    tnd::shared_ptr<Animation> m_animation;
    int m_posX;
    int m_posY;
    int m_speedX;
    int m_speedY;
};


#endif