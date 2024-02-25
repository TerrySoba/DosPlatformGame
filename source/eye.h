#ifndef _EYE_H_INCLUDED
#define _EYE_H_INCLUDED

#include "rectangle.h"
#include "animation.h"


class Eye
{
public:
    Eye(Rectangle enemyWalkArea, tnd::shared_ptr<Animation> animation);
    ~Eye();

    void walk(const Rectangle& playerPos);

    Rectangle getPos();

private:
    Rectangle m_enemyWalkArea;
    tnd::shared_ptr<Animation> m_animation;
    int m_posX;
    int m_posY;
    int m_middleX;
};


#endif