#ifndef _TENTACLE_H_INCLUDED
#define _TENTACLE_H_INCLUDED

#include "rectangle.h"
#include "animation.h"

class Tentacle
{
public:
    Tentacle(int posX, int posY, tnd::shared_ptr<Animation> animation);
    ~Tentacle();

    void act();

    Rectangle getPos();

private:
    tnd::shared_ptr<Animation> m_animation;
    int m_posX;
    int m_posY;
};


#endif