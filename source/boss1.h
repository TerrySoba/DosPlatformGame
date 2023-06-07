#ifndef _BOSS1_H_INCLUDED
#define _BOSS1_H_INCLUDED

#include "rectangle.h"
#include "animation.h"
#include "shared_ptr.h"
#include "vector.h"

enum Boss1State {
    BOSS1_STATE_INITIAL = 1,
    BOSS1_STATE_PROJECTILE_ATTACK = 2,
};

struct Projectile
{
    int16_t x;
    int16_t y;
    int16_t dx;
    int16_t dy;
};

class Boss1
{
public:
    Boss1(Rectangle enemyRectangle, shared_ptr<Animation> animation);
    virtual ~Boss1();

    void walk(const Rectangle& playerPos);

    tnd::vector<Rectangle> getProjectiles() const;

private:
    void moveProjectiles();

private:
    Rectangle m_enemyRectangle;
    shared_ptr<Animation> m_animation;
    Boss1State m_state;
    int16_t m_idleFrames;
    tnd::vector<Projectile> m_projectiles;
    int16_t m_lastPlayerPosX;
    int16_t m_lastPlayerPosY;
};


#endif