#include "tentacle.h"
#include "physics.h"

Tentacle::Tentacle(int posX, int posY, tnd::shared_ptr<Animation> animation)
{
    m_animation = animation;
    m_posX = posX;
    m_posY = posY;
    m_frame = 0;
    m_projectileLockoutTimer = 0;
    m_frame = 0;
}

Tentacle::~Tentacle()
{

}

const int PROJECTILE_SIZE = PIXEL_TO_SUBPIXEL(7);
const int SCREEN_WIDTH = 320;
const int SCREEN_HEIGHT = 200;
const int GRAVITY = 1;

const int16_t SPEED_TABLE[] = {10,-20,-15,5,17,-1, 1, 4, 3, 2 };
const int SPEED_TABLE_SIZE = sizeof(SPEED_TABLE) / sizeof(SPEED_TABLE[0]);

void Tentacle::act()
{
    if (m_projectileLockoutTimer > 0)
    {
        --m_projectileLockoutTimer;
    }
   
    if (m_projectiles.size() < 5 && m_projectileLockoutTimer == 0)
    {
        m_projectiles.push_back(Rectangle(m_posX + PIXEL_TO_SUBPIXEL(4), m_posY - PIXEL_TO_SUBPIXEL(0), PROJECTILE_SIZE, PROJECTILE_SIZE));
        
        if (++m_frame >= SPEED_TABLE_SIZE)
        {
            m_frame = 0;
        }
        m_projectileSpeeds.push_back(Point(SPEED_TABLE[m_frame], -60));

        m_projectileLockoutTimer = 10;
    }

    // apply gravity
    for (size_t i = 0; i < m_projectiles.size(); ++i)
    {
        m_projectileSpeeds[i].y += GRAVITY;
    }

    // Move projectiles
    for (size_t i = 0; i < m_projectiles.size(); ++i)
    {
        m_projectiles[i].x += m_projectileSpeeds[i].x;
        m_projectiles[i].y += m_projectileSpeeds[i].y;
    }

    // Check if projectiles are out of screen
    for (size_t i = 0; i < m_projectiles.size(); ++i)
    {
        if (m_projectiles[i].x + PROJECTILE_SIZE < 0 || m_projectiles[i].x > PIXEL_TO_SUBPIXEL(SCREEN_WIDTH) ||
            m_projectiles[i].y + PROJECTILE_SIZE < 0 || m_projectiles[i].y > PIXEL_TO_SUBPIXEL(SCREEN_HEIGHT))
        {
            m_projectiles.erase(i);
            m_projectileSpeeds.erase(i);
            --i;
        }
    }

}

tnd::vector<Rectangle> Tentacle::getProjectiles() const
{
    return m_projectiles;
}

Rectangle Tentacle::getPos()
{
    Rectangle rect(m_posX, m_posY, m_animation->width() * 16, m_animation->height() * 16);
    return rect;
}
