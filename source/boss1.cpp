#include "boss1.h"

#include "physics.h"

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 200
#define PROJECTILE_SPEED 40

template <class T>
T abs(T val)
{
    return val < 0 ? -val : val;
}

int32_t manhattanNorm(int32_t x, int32_t y)
{
    return abs(x) + abs(y);
}

Boss1::Boss1(Rectangle enemyRectangle, shared_ptr<Animation> animation) :
    m_enemyRectangle(enemyRectangle),
    m_animation(animation),
    m_state(BOSS1_STATE_INITIAL),
    m_idleFrames(0),
    m_lastPlayerPosX(-1),
    m_lastPlayerPosY(-1)
{
}

Boss1::~Boss1()
{

}

void Boss1::walk(const Rectangle& playerPos)
{
    moveProjectiles();

    switch (m_state)
    {   
        case BOSS1_STATE_INITIAL:
            // if distance between player and boss is less than 50 pixels enter projectile phase
            if (SUBPIXEL_TO_PIXEL(abs(playerPos.x - m_enemyRectangle.x)) < 100)
            {
                m_state = BOSS1_STATE_PROJECTILE_ATTACK;
            }
            break;
        case BOSS1_STATE_PROJECTILE_ATTACK:
            if (m_projectiles.size() >= 2)
            {
                m_state = BOSS1_STATE_INITIAL;
            }
            else
            {
                if (--m_idleFrames <= 0)
                {
                    m_idleFrames = 10;
                    // shoot projectiles
                    Projectile projectile;
                    projectile.x = m_enemyRectangle.x + m_enemyRectangle.width / 2 - PIXEL_TO_SUBPIXEL(m_animation->width()) / 2;
                    projectile.y = m_enemyRectangle.y + m_enemyRectangle.height / 2 - PIXEL_TO_SUBPIXEL(m_animation->height()) / 2;

                    // calculate expected player pos in 20 frames
                    int16_t playerPosX = playerPos.x; // + playerDx * 20;
                    int16_t playerPosY = playerPos.y; // + playerDy * 20;

                    // if last player position is positive predict movement
                    if (m_lastPlayerPosX >= 0 && m_lastPlayerPosY >= 0)
                    {
                        playerPosX += (playerPosX - m_lastPlayerPosX) * 20;
                        playerPosY += (playerPosY - m_lastPlayerPosY) * 20;
                    }

                    int32_t dx = playerPosX - projectile.x;
                    int32_t dy = playerPosY - projectile.y;
                    int32_t len = manhattanNorm(dx, dy);
                    if (len != 0)
                    {
                        projectile.dx = dx * PROJECTILE_SPEED / len;
                        projectile.dy = dy * PROJECTILE_SPEED / len;
                        m_projectiles.push_back(projectile);
                    }
                }
            }
            break;
    }

    m_lastPlayerPosX = playerPos.x;
    m_lastPlayerPosY = playerPos.y;
}

tnd::vector<Rectangle> Boss1::getProjectiles() const
{
    tnd::vector<Rectangle> projectiles;
    for (size_t i = 0; i < m_projectiles.size(); ++i)
    {
        Rectangle projectile;
        projectile.x = m_projectiles[i].x;
        projectile.y = m_projectiles[i].y;
        projectile.width = PIXEL_TO_SUBPIXEL(m_animation->width());
        projectile.height = PIXEL_TO_SUBPIXEL(m_animation->height());
        projectiles.push_back(projectile);
    }
    return projectiles;
}

void Boss1::moveProjectiles()
{
    for (size_t i = 0; i < m_projectiles.size(); ++i)
    {
        m_projectiles[i].x += m_projectiles[i].dx;
        m_projectiles[i].y += m_projectiles[i].dy;
    }

    // remove projectiles when they move out of the screen
    for (size_t i = 0; i < m_projectiles.size(); ++i)
    {
        if (SUBPIXEL_TO_PIXEL(m_projectiles[i].x) + m_animation->width() < 0  || SUBPIXEL_TO_PIXEL(m_projectiles[i].x) > SCREEN_WIDTH ||
            SUBPIXEL_TO_PIXEL(m_projectiles[i].y) + m_animation->height() < 0 || SUBPIXEL_TO_PIXEL(m_projectiles[i].y) > SCREEN_HEIGHT)
        {
            m_projectiles.erase(i);
            --i;
        }
    }
}
