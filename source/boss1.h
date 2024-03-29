#ifndef _BOSS1_H_INCLUDED
#define _BOSS1_H_INCLUDED

#include "rectangle.h"
#include "animation.h"
#include "vector.h"

enum Boss1State {
    BOSS1_STATE_INITIAL = 1,
    BOSS1_STATE_BURST_SHOT_PHASE = 2,
    BOSS1_STATE_WAVE_LINE_PHASE = 3,
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
    Boss1(Rectangle enemyRectangle, tnd::shared_ptr<Animation> animation, const tnd::vector<Rectangle>& walls);
    ~Boss1();

    void walk(const Rectangle& playerPos);

    tnd::vector<Rectangle> getProjectiles() const;

    void reset();

private:
    void moveProjectiles();

private:
    Rectangle m_enemyRectangle;
    tnd::shared_ptr<Animation> m_animation;
    Boss1State m_state;
    int16_t m_idleFrames;
    int16_t m_actionFrame;
    tnd::vector<Projectile> m_projectiles;
    tnd::vector<Rectangle> m_walls;
    int16_t m_lastPlayerPosX;
    int16_t m_lastPlayerPosY;
};


#endif