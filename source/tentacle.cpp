#include "tentacle.h"


Tentacle::Tentacle(int posX, int posY, tnd::shared_ptr<Animation> animation)
{
    m_animation = animation;
    m_posX = posX;
    m_posY = posY;
}

Tentacle::~Tentacle()
{

}

void Tentacle::act()
{

}

Rectangle Tentacle::getPos()
{
    Rectangle rect(m_posX, m_posY, m_animation->width() * 16, m_animation->height() * 16);
    return rect;
}
