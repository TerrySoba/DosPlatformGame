#ifndef _ENEMY_H_INCLUDED
#define _ENEMY_H_INCLUDED

#include "rectangle.h"
#include "animation.h"

class Enemy
{
public:
    Enemy(Rectangle enemyWalkArea, tnd::shared_ptr<Animation> animation);
    ~Enemy();

    void walk();

    Rectangle getPos();

private:
    Rectangle m_enemyWalkArea;
    tnd::shared_ptr<Animation> m_animation;
    int8_t m_walkSpeed;
    int m_posX;
    int m_posY;
};


#endif