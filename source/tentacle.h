#ifndef _TENTACLE_H_INCLUDED
#define _TENTACLE_H_INCLUDED

#include "rectangle.h"
#include "animation.h"

#include "vector.h"

class Tentacle
{
public:
    Tentacle(int posX, int posY, tnd::shared_ptr<Animation> animation);
    ~Tentacle();

    void act();

    Rectangle getPos();
    tnd::vector<Rectangle> getProjectiles() const;

private:
    tnd::shared_ptr<Animation> m_animation;
    int m_posX;
    int m_posY;
    tnd::vector<Rectangle> m_projectiles;
    tnd::vector<Point> m_projectileSpeeds;
    uint8_t m_projectileLockoutTimer;
    uint8_t m_frame;
};


#endif