#include "boss1.h"

#include "physics.h"
#include "vector_math.h"

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 200
#define PROJECTILE_SPEED 50

Boss1::Boss1(Rectangle enemyRectangle, tnd::shared_ptr<Animation> animation, const tnd::vector<Rectangle>& walls) :
    m_enemyRectangle(enemyRectangle),
    m_animation(animation),
    m_walls(walls)
{
    reset();
}

void Boss1::reset()
{
    m_projectiles.clear();
    m_state = BOSS1_STATE_INITIAL;
    m_idleFrames = 0;
    m_actionFrame = 0;
    m_lastPlayerPosX = -1;
    m_lastPlayerPosY = -1;
}


Boss1::~Boss1()
{

}




Projectile createProjectile(int16_t startX, int16_t startY, int16_t targetX, int16_t targetY, int16_t speed)
{
    Projectile projectile;
    projectile.x = startX;
    projectile.y = startY;

    int32_t dx = targetX - startX;
    int32_t dy = targetY - startY;
    setVectorLength(dx, dy, speed);
    projectile.dx = dx;
    projectile.dy = dy;
    return projectile;
}


const int16_t WIGGLE_TABLE[] = {1,2,3,4,5,6,5,4,3,2};
const int WIGGLE_TABLE_SIZE = sizeof(WIGGLE_TABLE) / sizeof(WIGGLE_TABLE[0]);

void Boss1::walk(const Rectangle& playerPos)
{
    moveProjectiles();

    switch (m_state)
    {   
        case BOSS1_STATE_INITIAL:
            m_state = BOSS1_STATE_BURST_SHOT_PHASE;
            m_idleFrames = 0;
            break;
        case BOSS1_STATE_BURST_SHOT_PHASE:
            if (SUBPIXEL_TO_PIXEL(playerPos.x) > 90)
            {
                m_state = BOSS1_STATE_WAVE_LINE_PHASE;
                m_idleFrames = 0;
            }
            break;
        case BOSS1_STATE_WAVE_LINE_PHASE:
            if (SUBPIXEL_TO_PIXEL(playerPos.x) < 80)
            {
                m_state = BOSS1_STATE_BURST_SHOT_PHASE;
                m_idleFrames = 0;
            }
            break;
    }

    if (m_idleFrames == 0)
    {
        if (m_state == BOSS1_STATE_WAVE_LINE_PHASE)
        {
            m_idleFrames = 7;
            ++m_actionFrame;

            if (m_actionFrame >= WIGGLE_TABLE_SIZE) m_actionFrame = 0;

            int16_t startX = m_enemyRectangle.x + m_enemyRectangle.width / 2 - PIXEL_TO_SUBPIXEL(m_animation->width()) / 2;
            int16_t startY = m_enemyRectangle.y + m_enemyRectangle.height / 2 - PIXEL_TO_SUBPIXEL(m_animation->height()) / 2;
            int16_t targetX = playerPos.x + playerPos.width / 2 + PIXEL_TO_SUBPIXEL(WIGGLE_TABLE[m_actionFrame]) * 10 + PIXEL_TO_SUBPIXEL(25);
            int16_t targetY = playerPos.y + playerPos.height / 2;
            m_projectiles.push_back(createProjectile(startX, startY, targetX, targetY, PROJECTILE_SPEED));
        } 
        else if (m_state == BOSS1_STATE_BURST_SHOT_PHASE)
        {
            m_idleFrames = 100;
        
            for (int i = -3; i < 3; ++i)
            {
                int16_t startX = m_enemyRectangle.x + m_enemyRectangle.width / 2 - PIXEL_TO_SUBPIXEL(m_animation->width()) / 2;
                int16_t startY = m_enemyRectangle.y + m_enemyRectangle.height / 2 - PIXEL_TO_SUBPIXEL(m_animation->height()) / 2;
                int16_t targetX = playerPos.x + playerPos.width / 2;
                int16_t targetY = playerPos.y + playerPos.height / 2 + PIXEL_TO_SUBPIXEL(i*10);
                m_projectiles.push_back(createProjectile(startX, startY, targetX, targetY, PROJECTILE_SPEED));
            }
        }
    }

    m_idleFrames = (m_idleFrames > 0) ? (m_idleFrames - 1) : 0;

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


bool intersectRect(const Rectangle &r1, const Rectangle &r2)
{
    return !(r2.x > r1.x + r1.width ||
             r2.x + r2.width < r1.x ||
             r2.y > r1.y + r1.height ||
             r2.y + r2.height < r1.y);
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

   
    // remove projectiles that intersect with walls
    for (size_t i = 0; i < m_projectiles.size(); ++i)
    {
        Rectangle projectile;
        projectile.x = m_projectiles[i].x;
        projectile.y = m_projectiles[i].y;
        projectile.width = PIXEL_TO_SUBPIXEL(m_animation->width());
        projectile.height = PIXEL_TO_SUBPIXEL(m_animation->height());

        projectile.shrink(PIXEL_TO_SUBPIXEL(2));

        for (size_t j = 0; j < m_walls.size(); ++j)
        {
            if (intersectRect(projectile, m_walls[j]))
            {
                m_projectiles.erase(i);
                --i;
                break;
            }
        }
    }

}
