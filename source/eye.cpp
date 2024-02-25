#include "eye.h"

Eye::Eye(Rectangle enemyWalkArea, tnd::shared_ptr<Animation> animation) :
    m_enemyWalkArea(enemyWalkArea), m_animation(animation)
{
    m_posX = enemyWalkArea.x;
    m_posY = enemyWalkArea.y + enemyWalkArea.height - animation->height() * 16;
    m_animation->useTag("WalkL");
    int minPosX = m_enemyWalkArea.x;
    int maxPosX = m_enemyWalkArea.x + m_enemyWalkArea.width - m_animation->width() * 16;
    m_middleX = (minPosX + maxPosX) / 2;
}

Eye::~Eye()
{
}

void Eye::walk(const Rectangle& playerPos)
{
    // calculate distance to player
    int distanceX = playerPos.x - m_posX;
    m_posX = m_middleX + distanceX / 16;
}

Rectangle Eye::getPos()
{
    return Rectangle(m_posX, m_posY, m_animation->width() * 16, m_animation->height() * 16);
}
