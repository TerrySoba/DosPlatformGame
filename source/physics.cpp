#include "physics.h"
#include "log.h"

#include <stdlib.h>

Physics::Physics(PhysicsCallback* callback) :
    m_callback(callback)
{
    m_rightLevelTransition = Rectangle(316, 0, 4, 200);
    m_rightLevelTransition *= 16;
    m_leftLevelTransition =  Rectangle(  0, 0, 4, 200); 
    m_leftLevelTransition *= 16;
    m_bottomLevelTransition = Rectangle(0, 196, 320, 4);
    m_bottomLevelTransition *= 16;
    m_topLevelTransition = Rectangle(0, 0, 320, 4);
    m_topLevelTransition *= 16;
}

int Physics::addActor(const Actor &rect)
{
    m_actors.push_back(rect);
    return m_actors.size() - 1;
}

void Physics::setActor(int index, const Actor &rect)
{
    m_actors[index] = rect;
}

void Physics::setWalls(const tnd::vector<Rectangle>& walls)
{
    m_walls = walls;
}

void Physics::setDeath(const tnd::vector<Rectangle>& death)
{
    m_death = death;
}

void Physics::setEnemyDeath(const tnd::vector<Rectangle>& death)
{
    m_enemyDeath = death;
}

void Physics::setFallThrough(const tnd::vector<Rectangle>& fallThrough)
{
    m_fallThrough = fallThrough;
}

void Physics::setGuffins(const tnd::vector<Rectangle>& guffins)
{
    m_guffins = guffins;
}

void Physics::setSpawnPoint(const Point& point)
{
    m_spawn = point;
    if (m_actors.size() > 0)
    {
        m_actors[0].rect.x = point.x;
        m_actors[0].rect.y = point.y;
    }
}

void Physics::getActorPos(int index, int16_t& x, int16_t& y)
{
    Actor& actor = m_actors[index];
    x = actor.rect.x;
    y = actor.rect.y;
}


Rectangle extendRectangle(Rectangle rect, int16_t horizontal, int16_t vertical)
{
    rect.x -= horizontal;
    rect.width += horizontal * 2;
    rect.y -= vertical;
    rect.height += vertical * 2;
    return rect;
}

void Physics::calc()
{
    for (int i = 0; i < m_actors.size(); ++i)
    {
        Actor& actor = m_actors[i];
        Point oldPos(actor.rect.x, actor.rect.y);

        actor.rect.x += actor.dx;
        actor.rect.y += actor.dy;

        actor.dx *= 0.8;
        actor.dy *= 0.99;

        actor.isOnGround = false;

        // check for level transitions

        if (m_callback)
        {
            if (intersectRect(actor.rect, m_rightLevelTransition))
            {
                m_callback->levelTransition(RIGHT);
            } 
            else if (intersectRect(actor.rect, m_leftLevelTransition))
            {
                m_callback->levelTransition(LEFT);
            }
            else if (intersectRect(actor.rect, m_bottomLevelTransition))
            {
                m_callback->levelTransition(BOTTOM);
            }
            else if (intersectRect(actor.rect, m_topLevelTransition))
            {
                m_callback->levelTransition(TOP);
            }
        }

        bool fallThrough = false;

        if (actor.isDucking)
        {
            for (int n = 0; n < m_fallThrough.size(); ++n)
            {
                if (intersectRect(m_fallThrough[n], actor.rect))
                {
                    fallThrough = true;
                }
            }
        }

        for (int n = 0; n < m_walls.size(); ++n)
        {
            Rectangle& wall = m_walls[n];
            if (intersectRect(wall, actor.rect))
            {
                IntersectionType wallType = getIntersectionType(extendRectangle(wall, 0, actor.rect.height - 32), actor.rect);
                if (!actor.isFallingThrough)
                {
                    switch(wallType)
                    {
                        case INTERSECTION_LEFT:
                            actor.dx = 0;
                            actor.rect.x = wall.x - actor.rect.width;
                            break;
                        case INTERSECTION_RIGHT:
                            actor.dx = 0;
                            actor.rect.x = wall.x + wall.width;
                            break;
                    }
                }

                IntersectionType groundType = getIntersectionType(extendRectangle(wall, actor.rect.width - 32, 0), actor.rect);
                switch(groundType)
                {
                    case INTERSECTION_TOP:
                        if (fallThrough)
                        {
                            actor.isFallingThrough = true;
                        }
                        if (!fallThrough && !actor.isFallingThrough)
                        {
                            actor.rect.y = wall.y - actor.rect.height;
                            actor.dy = 0;
                            actor.isOnGround = true;
                        }
                        break;
                    case INTERSECTION_BOTTOM:
                        actor.isFallingThrough = false;
                        actor.rect.y = wall.y + wall.height;
                        actor.dy = abs(actor.dy);
                        break;
                }
            }
        }

        for (int n = 0; n < m_death.size(); ++n)
        {
            Rectangle& death = m_death[n];
            if (intersectRect(death, actor.rect))
            {
                actor.rect.x = m_spawn.x;
                actor.rect.y = m_spawn.y;
                actor.dx = 0;
                actor.dy = 0;
            }
        }

        for (int n = 0; n < m_enemyDeath.size(); ++n)
        {
            Rectangle& death = m_enemyDeath[n];
            if (intersectRect(death, actor.rect))
            {
                actor.rect.x = m_spawn.x;
                actor.rect.y = m_spawn.y;
                actor.dx = 0;
                actor.dy = 0;
            }
        }


        for (int n = 0; n < m_guffins.size(); ++n)
        {
            Rectangle& guffin = m_guffins[n];
            if (intersectRect(guffin, actor.rect))
            {
                m_callback->collectApple(Point(guffin.x, guffin.y));
            }
        }

        actor.dy += 3;
        if (actor.dy > 32) actor.dy = 32;
    }
}

void Physics::startActorJump(int index)
{
    Actor& actor = m_actors[index];
    // if (actor.jumpFrame == 0) actor.jumpFrame = 1;
    if (actor.isOnGround) actor.dy = -64;
}

void Physics::stopActorJump(int index)
{

}

void Physics::setActorSpeedX(int index, int16_t dx)
{
    Actor& actor = m_actors[index];
    actor.dx = dx;
}

void Physics::setActorSpeedY(int index, int16_t dy)
{
    Actor& actor = m_actors[index];
    actor.dy = dy;
}

void Physics::setActorDuck(int index, bool isDucking)
{
    m_actors[index].isDucking = isDucking;
}

bool Physics::intersectRect(const Rectangle &r1, const Rectangle &r2)
{
    return !(r2.x > r1.x + r1.width ||
             r2.x + r2.width < r1.x ||
             r2.y > r1.y + r1.height ||
             r2.y + r2.height < r1.y);
}

bool pointIntersect(const Rectangle& r, int16_t x, int16_t y)
{
    return (x >= r.x && x <= r.x + r.width) &&
           (y >= r.y && y <= r.y + r.height);
}

IntersectionType Physics::getIntersectionType(const Rectangle &r1, const Rectangle &r2)
{
    bool topLeft = pointIntersect(r1, r2.x, r2.y);
    bool topRight = pointIntersect(r1, r2.x + r2.width, r2.y);
    bool bottomLeft = pointIntersect(r1, r2.x, r2.y + r2.height);
    bool bottomRight = pointIntersect(r1, r2.x + r2.width, r2.y + r2.height);

    if (bottomLeft && bottomRight && !topLeft && !topRight)
    {
        return INTERSECTION_TOP;
    }
    if (bottomLeft && !bottomRight && topLeft && !topRight)
    {
        return INTERSECTION_RIGHT;
    }
    if (!bottomLeft && bottomRight && !topLeft && topRight)
    {
        return INTERSECTION_LEFT;
    }
    if (!bottomLeft && !bottomRight && topLeft && topRight)
    {
        return INTERSECTION_BOTTOM;
    }


    return INTERSECTION_OTHER;
}
