#ifndef ACTOR_ANIMATION_CONTROLLER_H_INCLUDED
#define ACTOR_ANIMATION_CONTROLLER_H_INCLUDED

#include "animation.h"
#include "shared_ptr.h"

#include <stdlib.h>

enum AnimationEnum
{
    ANIM_INITIAL,
    ANIM_WALK_RIGHT,
    ANIM_WALK_LEFT,
    ANIM_STAND_RIGHT,
    ANIM_STAND_LEFT,
    ANIM_JUMP_RIGHT,
    ANIM_JUMP_LEFT,
};

enum VerticalDirection
{
    DIR_UP,
    DIR_DOWN
};

enum HorizontalDirection
{
    DIR_LEFT,
    DIR_RIGHT
};


class ActorAnimationController
{
public:
    ActorAnimationController(shared_ptr<Animation> actorAnimation) :
        m_actorAnimation(actorAnimation),
        m_lastX(0),
        m_lastY(0),
        m_activeAnimation(ANIM_INITIAL),
        m_lastDirection(DIR_DOWN),
        m_lastHDir(DIR_RIGHT),
        m_airFrames(0)
    {
    }
    
    void setPos(int16_t x, int16_t y)
    {
        int16_t dx = x - m_lastX;
        int16_t dy = y - m_lastY;
        m_lastX = x;
        m_lastY = y;

        VerticalDirection direction = (dy > 0) ? DIR_DOWN : DIR_UP;

        AnimationEnum nextAnim = m_activeAnimation;

        if (abs(dy) < 2)
        {
            if (m_lastDirection == DIR_DOWN || m_airFrames++ > 4)
            {
                m_airFrames = 0;
                if (dx > 0)
                {
                    m_lastHDir = DIR_RIGHT;
                    nextAnim = ANIM_WALK_RIGHT;
                }
                else if (dx < 0)
                {
                    m_lastHDir = DIR_LEFT;
                    nextAnim = ANIM_WALK_LEFT;
                }
                else if (dx == 0)
                {
                    if (m_lastHDir == DIR_RIGHT) nextAnim = ANIM_STAND_RIGHT;
                    else nextAnim = ANIM_STAND_LEFT;
                }
            }
            else
            {
                ++m_airFrames;
                nextAnim = m_activeAnimation;
            }
        }
        else
        {
            if (dx > 0)
            {
                m_lastHDir = DIR_RIGHT;
                nextAnim = ANIM_JUMP_RIGHT;
            }
            else if (dx < 0)
            {
                m_lastHDir = DIR_LEFT;
                nextAnim = ANIM_JUMP_LEFT;
            }
            else
            {
                if (m_lastHDir == DIR_RIGHT) nextAnim = ANIM_JUMP_RIGHT;
                else nextAnim = ANIM_JUMP_LEFT;
            }
        }
        
        if (dy != 0) m_lastDirection = direction;

        if (nextAnim != m_activeAnimation)
        {
            m_activeAnimation = nextAnim;
            switch (nextAnim)
            {
            case ANIM_WALK_RIGHT:
                m_actorAnimation->useTag("WalkR");
                break;
            case ANIM_WALK_LEFT:
                m_actorAnimation->useTag("WalkL");
                break;
            case ANIM_STAND_RIGHT:
                m_actorAnimation->useTag("StandR");
                break;
            case ANIM_STAND_LEFT:
                m_actorAnimation->useTag("StandL");
                break;
            case ANIM_JUMP_RIGHT:
                m_actorAnimation->useTag("JumpR");
                break;
            case ANIM_JUMP_LEFT:
                m_actorAnimation->useTag("JumpL");
                break;
            }
        }

    }

private:
    shared_ptr<Animation> m_actorAnimation;
    int16_t m_lastX;
    int16_t m_lastY;
    AnimationEnum m_activeAnimation;
    VerticalDirection m_lastDirection;
    HorizontalDirection m_lastHDir;
    int m_airFrames;
};

#endif