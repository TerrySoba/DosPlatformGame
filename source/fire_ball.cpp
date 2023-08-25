#include "fire_ball.h"
#include "physics.h"
#include <stdlib.h>

FireBall::FireBall(Rectangle collision, tnd::shared_ptr<Animation> animation) :
    m_collision(collision),
    m_animation(animation),
    m_posX(collision.x),
    m_posY(collision.y),
    m_speedX(16),
    m_speedY(13)
{

}

FireBall::~FireBall()
{

}

void FireBall::walk()
{
    m_posX += m_speedX;
    m_posY += m_speedY;

    if (m_posX < m_collision.x) {
        m_posX = m_collision.x;
        m_speedX = abs(m_speedX);
    }
    if (m_posY < m_collision.y) {
        m_posY = m_collision.y;
        m_speedY = abs(m_speedY);
    }

    if (m_posX > m_collision.x + m_collision.width - PIXEL_TO_SUBPIXEL(m_animation->width())) {
        m_posX = m_collision.x + m_collision.width - PIXEL_TO_SUBPIXEL(m_animation->width());
        m_speedX = -abs(m_speedX);
    }

    if (m_posY > m_collision.y + m_collision.height - PIXEL_TO_SUBPIXEL(m_animation->height())) {
        m_posY = m_collision.y + m_collision.height - PIXEL_TO_SUBPIXEL(m_animation->height());
        m_speedY = -abs(m_speedY);
    }
}

Rectangle FireBall::getPos()
{
    return Rectangle(m_posX, m_posY, PIXEL_TO_SUBPIXEL(m_animation->width()), PIXEL_TO_SUBPIXEL(m_animation->height()));
}