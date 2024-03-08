#include "boss2.h"

#include "subpixels.h"
#include "vector_math.h"

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 200

Boss2::Boss2(Rectangle enemyRectangle, tnd::shared_ptr<Animation> animation, const tnd::vector<Rectangle>& walls) :
    m_enemyRectangle(enemyRectangle),
    m_animation(animation),
    m_walls(walls)
{
    reset();
}

Boss2::~Boss2()
{

}

void addProjectileLine(
    tnd::vector<Boss2Projectile> &projectiles,
    int16_t count,
    int16_t startX,
    int16_t startY,
    int16_t endX,
    int16_t endY,
    int16_t speed)
{
    int16_t dx = (endX - startX) / count;
    int16_t dy = (endY - startY) / count;

    int16_t dirX = endX - startX;
    int16_t dirY = endY - startY;

    makeOrthogonal(dirX, dirY);
    setVectorLength(dirX, dirY, speed);
    
    for (int i = 0; i < count; ++i)
    {
        Boss2Projectile projectile;
        projectile.x = startX + i * dx;
        projectile.y = startY + i * dy;

        projectile.dx = dirX;
        projectile.dy = dirY;        

        projectiles.push_back(projectile);
    }
}


bool playerInLeftSector(const Rectangle& playerPos)
{
    return playerPos.x < PIXEL_TO_SUBPIXEL(SCREEN_WIDTH / 3);
}

bool playerInTopRightSector(const Rectangle& playerPos)
{
    return (playerPos.x > PIXEL_TO_SUBPIXEL(SCREEN_WIDTH - (SCREEN_WIDTH / 3))) &&
           (playerPos.y < PIXEL_TO_SUBPIXEL(SCREEN_HEIGHT / 2));
}

void Boss2::walk(const Rectangle& playerPos)
{
    moveProjectiles();

    switch(m_state)
    {
        case BOSS2_STATE_SPAWN_VLINE:
        {
            m_state = BOSS2_STATE_WAIT_VLINE;

            addProjectileLine(
                m_projectiles,
                2,
                PIXEL_TO_SUBPIXEL(0), PIXEL_TO_SUBPIXEL( 90),
                PIXEL_TO_SUBPIXEL(0), PIXEL_TO_SUBPIXEL(110),
                -60);
            addProjectileLine(
                m_projectiles,
                2,
                PIXEL_TO_SUBPIXEL(0), PIXEL_TO_SUBPIXEL(140),
                PIXEL_TO_SUBPIXEL(0), PIXEL_TO_SUBPIXEL(160),
                -60);
            break;
        }
        case BOSS2_STATE_WAIT_VLINE:
        {
            if (m_projectiles.size() == 0)
            {
                if (playerInLeftSector(playerPos))
                {
                    m_state = BOSS2_STATE_SPAWN_HLINE;
                }
                else if (playerInTopRightSector(playerPos))
                {
                    m_state = BOSS2_STATE_SPAWN_TOP_VLINE;
                }
                else
                {
                    m_state = BOSS2_STATE_SPAWN_VLINE2;
                }
            }
            break;
        }
        case BOSS2_STATE_SPAWN_VLINE2:
        {
            m_state = BOSS2_STATE_WAIT_VLINE2;
            addProjectileLine(
                m_projectiles,
                2,
                PIXEL_TO_SUBPIXEL(0), PIXEL_TO_SUBPIXEL( 90),
                PIXEL_TO_SUBPIXEL(0), PIXEL_TO_SUBPIXEL(110),
                -60);
            addProjectileLine(
                m_projectiles,
                2,
                PIXEL_TO_SUBPIXEL(0), PIXEL_TO_SUBPIXEL(135),
                PIXEL_TO_SUBPIXEL(0), PIXEL_TO_SUBPIXEL(160),
                -60);
            break;
        }
        case BOSS2_STATE_WAIT_VLINE2:
        {
            if (m_projectiles.size() == 0)
            {
                if (playerInLeftSector(playerPos))
                {
                    m_state = BOSS2_STATE_SPAWN_HLINE;
                }
                else if (playerInTopRightSector(playerPos))
                {
                    m_state = BOSS2_STATE_SPAWN_TOP_VLINE;
                }
                else
                {
                    m_state = BOSS2_STATE_SPAWN_VLINE;
                }
            }
            break;
        }
        case BOSS2_STATE_SPAWN_HLINE:
        {
            m_state = BOSS2_STATE_WAIT_VLINE2;
            addProjectileLine(
                m_projectiles,
                3,
                PIXEL_TO_SUBPIXEL(0), PIXEL_TO_SUBPIXEL(200),
                PIXEL_TO_SUBPIXEL(100), PIXEL_TO_SUBPIXEL(200),
                -80);
            
            break;
        }
        case BOSS2_STATE_SPAWN_TOP_VLINE:
        {
            m_state = BOSS2_STATE_WAIT_TOP_VLINE;
            addProjectileLine(
                m_projectiles,
                3,
                PIXEL_TO_SUBPIXEL(300), PIXEL_TO_SUBPIXEL(  1),
                PIXEL_TO_SUBPIXEL(300), PIXEL_TO_SUBPIXEL( 40),
                15);
            break;
        }
        case BOSS2_STATE_WAIT_TOP_VLINE:
        {
            if (m_projectiles.size() == 0)
            {
                if (playerInLeftSector(playerPos))
                {
                    m_state = BOSS2_STATE_SPAWN_HLINE;
                }
                else if (playerInTopRightSector(playerPos))
                {
                    m_state = BOSS2_STATE_SPAWN_TOP_VLINE;
                }
                else
                {
                    m_state = BOSS2_STATE_SPAWN_VLINE;
                }
            }
            break;
        }
    }
}

tnd::vector<Rectangle> Boss2::getProjectiles() const
{
    tnd::vector<Rectangle> generatedRectangles;
    for (int i = 0; i < m_projectiles.size(); ++i)
    {
        const Boss2Projectile& projectile = m_projectiles[i];
        Rectangle rect(projectile.x, projectile.y, PIXEL_TO_SUBPIXEL(m_animation->width()), PIXEL_TO_SUBPIXEL(m_animation->height()));
        generatedRectangles.push_back(rect);
    }
    return generatedRectangles;
}

void Boss2::reset()
{
    m_state = BOSS2_STATE_SPAWN_VLINE;
    m_projectiles.clear();
}


void Boss2::moveProjectiles()
{
    for (int i = 0; i < m_projectiles.size(); ++i)
    {
        Boss2Projectile& projectile = m_projectiles[i];
        projectile.x += projectile.dx;
        projectile.y += projectile.dy;
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