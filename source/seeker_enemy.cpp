#include "seeker_enemy.h"

SeekerEnemy::SeekerEnemy(Rectangle enemyWalkArea, shared_ptr<Animation> animation) :
    m_enemyWalkArea(enemyWalkArea), m_animation(animation), m_walkSpeed(10)
{
    m_posX = enemyWalkArea.x;
    m_posY = enemyWalkArea.y + enemyWalkArea.height - animation->height() * 16;
}

SeekerEnemy::~SeekerEnemy()
{

}

void SeekerEnemy::walk(const Rectangle& playerPos)
{
    if (m_posX < playerPos.x)
    {
        m_posX += m_walkSpeed;
    }
    else if (m_posX > playerPos.x)
    {
        m_posX -= m_walkSpeed;
    }


    if (m_posX < m_enemyWalkArea.x)
    {
        m_posX = m_enemyWalkArea.x;
    }

    if (m_posX  > m_enemyWalkArea.x + m_enemyWalkArea.width - m_animation->width() * 16)
    {
        m_posX = m_enemyWalkArea.x + m_enemyWalkArea.width - m_animation->width() * 16;
    }


}

Rectangle SeekerEnemy::getPos()
{
    return Rectangle(m_posX, m_posY, m_animation->width() * 16, m_animation->height() * 16);
}
