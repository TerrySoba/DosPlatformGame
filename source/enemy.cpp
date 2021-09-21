#include "enemy.h"

#include <stdlib.h>

Enemy::Enemy(Rectangle enemyWalkArea, shared_ptr<Animation> animation) :
    m_enemyWalkArea(enemyWalkArea), m_animation(animation), m_walkSpeed(24)
{
    m_posY = m_enemyWalkArea.y;
    m_posX = m_enemyWalkArea.x;

    // shrink rectangle by animation width, so that we only can ignore animation width later on
    m_enemyWalkArea.width -= animation->width() * 16;

}

void Enemy::walk()
{
    m_posX += m_walkSpeed;
    if (m_posX > m_enemyWalkArea.x + m_enemyWalkArea.width)
    {
        m_walkSpeed = -abs(m_walkSpeed);
    }
    if (m_posX < m_enemyWalkArea.x)
    {
        m_walkSpeed = abs(m_walkSpeed);
    }
}

Rectangle Enemy::getPos()
{
    return Rectangle(m_posX, m_posY, m_animation->width() * 16, m_animation->height() * 16);
}

Enemy::~Enemy()
{
}
