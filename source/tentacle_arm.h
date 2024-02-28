#ifndef _TENTACLE_ARM_H_INCLUDED
#define _TENTACLE_ARM_H_INCLUDED

#include "rectangle.h"
#include "animation.h"

class TentacleArm
{
public:
    TentacleArm(int posX, int posY, tnd::shared_ptr<Animation> animation);
    ~TentacleArm();

    void act(int actorPosX, int actorPosY);

    tnd::vector<Rectangle> getSegments();

    void reset();

private:
    int m_posX;
    int m_posY;

    tnd::shared_ptr<Animation> m_animation;
    tnd::vector<Rectangle> m_segments;

    int m_armExtension;

    uint16_t m_wiggleFrame;
};


#endif