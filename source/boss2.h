#ifndef _BOSS2_H_INCLUDED
#define _BOSS2_H_INCLUDED

#include "rectangle.h"
#include "animation.h"
#include "vector.h"

enum Boss2State {
    BOSS2_STATE_SPAWN_VLINE = 1,
    BOSS2_STATE_WAIT_VLINE = 2,
    BOSS2_STATE_SPAWN_VLINE2 = 3,
    BOSS2_STATE_WAIT_VLINE2 = 4,
    BOSS2_STATE_SPAWN_HLINE = 5,
    BOSS2_STATE_WAIT_HLINE = 6,
    BOSS2_STATE_SPAWN_TOP_VLINE = 7,
    BOSS2_STATE_WAIT_TOP_VLINE = 8,
};

struct Boss2Projectile
{
    int16_t x;
    int16_t y;
    int16_t dx;
    int16_t dy;
};

class Boss2
{
public:
    Boss2(Rectangle enemyRectangle, tnd::shared_ptr<Animation> animation, const tnd::vector<Rectangle>& walls);
    ~Boss2();

    void walk(const Rectangle& playerPos);

    tnd::vector<Rectangle> getProjectiles() const;

    void reset();

private:
    void moveProjectiles();

private:
    Rectangle m_enemyRectangle;
    tnd::shared_ptr<Animation> m_animation;
    Boss2State m_state;
    tnd::vector<Boss2Projectile> m_projectiles;
    tnd::vector<Rectangle> m_walls;
};


#endif