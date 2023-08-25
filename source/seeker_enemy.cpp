#include "seeker_enemy.h"

SeekerEnemy::SeekerEnemy(Rectangle enemyWalkArea, tnd::shared_ptr<Animation> animation) :
    m_enemyWalkArea(enemyWalkArea), m_animation(animation), m_walkSpeed(10), m_oldDirection(WALK_LEFT), m_direction(WALK_RIGHT), m_firstLoop(true)
{
    m_posX = enemyWalkArea.x;
    m_posY = enemyWalkArea.y + enemyWalkArea.height - animation->height() * 16;
    m_animation->useTag("WalkL");
}

SeekerEnemy::~SeekerEnemy()
{

}

void SeekerEnemy::walk(const Rectangle& playerPos)
{
    m_oldDirection = m_direction;
    

    if (m_posX < playerPos.x)
    {
        m_posX += m_walkSpeed;
        m_direction = WALK_RIGHT;
    }
    else if (m_posX > playerPos.x)
    {
        m_posX -= m_walkSpeed;
        m_direction = WALK_LEFT;
    }


    if (m_posX < m_enemyWalkArea.x)
    {
        m_posX = m_enemyWalkArea.x;
    }

    if (m_posX  > m_enemyWalkArea.x + m_enemyWalkArea.width - m_animation->width() * 16)
    {
        m_posX = m_enemyWalkArea.x + m_enemyWalkArea.width - m_animation->width() * 16;
    }

    if ((m_oldDirection != m_direction) || m_firstLoop)
    {
        m_firstLoop = false;
        switch (m_direction)
        {
        case WALK_LEFT:
            m_animation->useTag("WalkL");
            break;
        case WALK_RIGHT:
            m_animation->useTag("WalkR");
            break;
        }
    }

}

Rectangle SeekerEnemy::getPos()
{
    return Rectangle(m_posX, m_posY, m_animation->width() * 16, m_animation->height() * 16);
}
