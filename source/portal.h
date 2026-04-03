#ifndef _PORTAL_H_INCLUDED
#define _PORTAL_H_INCLUDED

#include "rectangle.h"
#include "animation.h"
#include "vector.h"
#include "level_number.h"

class Portal
{
public:

    Portal() :
        m_collision(0, 0, 0, 0), m_targetLevel(0, 0)
    {}

    Portal(const Rectangle& collision, uint16_t targetLevelX, uint16_t targetLevelY) :
        m_collision(collision), m_targetLevel(targetLevelX, targetLevelY)
    {}

    Rectangle getPos() { return m_collision; }
    LevelNumber getTargetLevel() { return m_targetLevel; }

private:
    Rectangle m_collision;
    LevelNumber m_targetLevel;
};


#endif