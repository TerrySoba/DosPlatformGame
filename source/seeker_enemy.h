#ifndef _SEEKER_ENEMY_H_INCLUDED
#define _SEEKER_ENEMY_H_INCLUDED

#include "rectangle.h"
#include "animation.h"
#include "shared_ptr.h"

enum WalkingDirection
{
    WALK_RIGHT,
    WALK_LEFT
};

class SeekerEnemy
{
public:
    SeekerEnemy(Rectangle enemyWalkArea, shared_ptr<Animation> animation);
    ~SeekerEnemy();

    void walk(const Rectangle& playerPos);

    Rectangle getPos();

private:
    Rectangle m_enemyWalkArea;
    shared_ptr<Animation> m_animation;
    int8_t m_walkSpeed;
    int m_posX;
    int m_posY;

    WalkingDirection m_oldDirection;
    WalkingDirection m_direction;
    bool m_firstLoop;
};


#endif