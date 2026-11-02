#include "physics.h"

#include <stdlib.h>

int16_t JETPACK_THRUST_TABLE[] = {
    1,1,2,2,2,3,3,3,3,3,
    1,1,1,1,1,3,4,5,4,5,
    3,3,2,2,5,5,1,1,4,4,
    5,5,5,5,5,5,5,5,5,5,
};

static const int16_t JETPACK_THRUST_TABLE_SIZE =
    sizeof(JETPACK_THRUST_TABLE) / sizeof(JETPACK_THRUST_TABLE[0]);


Physics::Physics(PhysicsCallback* callback, tnd::shared_ptr<SoundController> sound) :
    m_callback(callback), m_sound(sound), m_jetpackActive(false), m_lastFrameJetpackActive(false)
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

Physics::~Physics()
{
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

void Physics::setCreditsWarps(const tnd::vector<Rectangle>& creditsWarps)
{
    m_creditsWarps = creditsWarps;
}

void Physics::setEnemyDeath(const tnd::vector<Rectangle>& death)
{
    // called every frame, so reuse the existing buffer
    m_enemyDeath.assign(death);
}

void Physics::setFallThrough(const tnd::vector<Rectangle>& fallThrough)
{
    m_fallThrough = fallThrough;
}

void Physics::setGuffins(const tnd::vector<Rectangle>& guffins)
{
    m_guffins = guffins;
}

void Physics::setJetPacks(const tnd::vector<Rectangle>& jetPacks)
{
    m_jetPacks = jetPacks;
}

void Physics::setSunItems(const tnd::vector<Rectangle>& sunItems)
{
    m_sunItems = sunItems;
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

void Physics::setSunPoint(const Point &point)
{
    m_sun = point;
}

void Physics::setButtons(const tnd::vector<Button>& buttons)
{
    m_buttons = buttons;
}

void Physics::setPortals(const tnd::vector<Portal>& portals)
{
    m_portals = portals;
}

void Physics::getActorPos(int index, int16_t& x, int16_t& y)
{
    Actor& actor = m_actors[index];
    x = actor.rect.x;
    y = actor.rect.y;
}


// Returns the index of the first rectangle in rects starting at index start that intersects bounds, or -1.
static int nextIntersectingRect(const tnd::vector<Rectangle>& rects, int start, const RectBounds& bounds)
{
    return findIntersectingRect(rects.data(), rects.size(), start, bounds);
}

static bool containsRect(const tnd::vector<Rectangle>& rects, const Rectangle& rect)
{
    for (int i = 0; i < rects.size(); ++i)
    {
        if (rects[i] == rect)
        {
            return true;
        }
    }
    return false;
}

void Physics::calc()
{
    m_lastFrameJetpackActive = m_jetpackActive;
    m_jetpackActive = false;
    for (int i = 0; i < m_actors.size(); ++i)
    {
        Actor& actor = m_actors[i];
        Point oldPos(actor.rect.x, actor.rect.y);

        actor.rect.x += actor.dx;
        actor.rect.y += actor.dy;

        // Integer versions of "dx *= 0.8" and "dy *= 0.99" with identical results, as floating point
        // is emulated in software on machines without FPU. dx is only ever set to small values,
        // so 16 bit math can not overflow. dy is not limited in negative direction, so it uses 32 bit math.
        actor.dx = (actor.dx * 4) / 5;
        actor.dy = (int16_t)(((int32_t)actor.dy * 99) / 100);

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

        // needs to be updated whenever actor.rect changes
        RectBounds actorBounds(actor.rect);

        bool touchesFallThrough = nextIntersectingRect(m_fallThrough, 0, actorBounds) >= 0;

        // falling through is over as soon as the actor does not touch any fall through platform anymore
        if (!touchesFallThrough)
        {
            actor.isFallingThrough = false;
        }

        bool fallThrough = actor.isDucking && touchesFallThrough;
        bool isOnSolidGround = false;

        for (int n = nextIntersectingRect(m_walls, 0, actorBounds);
             n >= 0;
             n = nextIntersectingRect(m_walls, n + 1, actorBounds))
        {
            Rectangle& wall = m_walls[n];
            // fall through platforms are also part of the walls, only those may be fallen through
            bool isFallThroughWall = touchesFallThrough && containsRect(m_fallThrough, wall);
            bool ignoreWall = isFallThroughWall && actor.isFallingThrough;

            IntersectionType wallType = getIntersectionType(wall, 0, actor.rect.height - 32, actor.rect);
            if (!ignoreWall)
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

            IntersectionType groundType = getIntersectionType(wall, actor.rect.width - 32, 0, actor.rect);
            switch(groundType)
            {
                case INTERSECTION_TOP:
                    if (isFallThroughWall && fallThrough)
                    {
                        actor.isFallingThrough = true;
                        ignoreWall = true;
                    }
                    if (!ignoreWall)
                    {
                        actor.rect.y = wall.y - actor.rect.height;
                        actor.dy = 0;
                        actor.isOnGround = true;
                        actor.jetpackFrames = JETPACK_THRUST_TABLE_SIZE - 1; // reload jetpack after touching ground
                        if (!isFallThroughWall)
                        {
                            isOnSolidGround = true;
                        }
                    }
                    break;
                case INTERSECTION_BOTTOM:
                    actor.isFallingThrough = false;
                    actor.rect.y = wall.y + wall.height;
                    actor.dy = abs(actor.dy);
                    break;
            }

            // the actor may have been moved by the collision handling above
            actorBounds = RectBounds(actor.rect);
        }

        // standing on solid ground without ducking, so do not fall through any platform touched later
        if (isOnSolidGround && !fallThrough)
        {
            actor.isFallingThrough = false;
        }

        for (int n = nextIntersectingRect(m_creditsWarps, 0, actorBounds);
             n >= 0;
             n = nextIntersectingRect(m_creditsWarps, n + 1, actorBounds))
        {
            if (m_callback)
            {
                m_callback->onCreditsWarp();
            }
        }

        for (int n = nextIntersectingRect(m_death, 0, actorBounds);
             n >= 0;
             n = nextIntersectingRect(m_death, n + 1, actorBounds))
        {
            actor.rect.x = m_spawn.x;
            actor.rect.y = m_spawn.y;
            actor.dx = 0;
            actor.dy = 0;
            actor.isFallingThrough = false;
            actorBounds = RectBounds(actor.rect);
            m_sound->playDeathSound();
            if (m_callback)
            {
                m_callback->onDeath();
            }
        }

        for (int n = nextIntersectingRect(m_enemyDeath, 0, actorBounds);
             n >= 0;
             n = nextIntersectingRect(m_enemyDeath, n + 1, actorBounds))
        {
            if (m_callback && !m_callback->confirmEnemyCollision(n, actor.rect))
            {
                continue;
            }

            actor.rect.x = m_spawn.x;
            actor.rect.y = m_spawn.y;
            actor.dx = 0;
            actor.dy = 0;
            actor.isFallingThrough = false;
            actorBounds = RectBounds(actor.rect);
            m_sound->playDeathSound();
            if (m_callback)
            {
                m_callback->onDeath();
            }
        }

        // The collect callbacks remove the item from the list, so the vector is read again for every search.
        if (m_callback)
        {
            for (int n = nextIntersectingRect(m_guffins, 0, actorBounds);
                 n >= 0;
                 n = nextIntersectingRect(m_guffins, n + 1, actorBounds))
            {
                m_callback->collectApple(Point(m_guffins[n].x, m_guffins[n].y));
            }

            for (int n = nextIntersectingRect(m_jetPacks, 0, actorBounds);
                 n >= 0;
                 n = nextIntersectingRect(m_jetPacks, n + 1, actorBounds))
            {
                m_callback->collectJetPack(Point(m_jetPacks[n].x, m_jetPacks[n].y));
            }

            for (int n = nextIntersectingRect(m_sunItems, 0, actorBounds);
                 n >= 0;
                 n = nextIntersectingRect(m_sunItems, n + 1, actorBounds))
            {
                m_callback->collectSunItem(Point(m_sunItems[n].x, m_sunItems[n].y));
            }
        }

        for (int n = 0; n < m_buttons.size(); ++n)
        {
            Button& button = m_buttons[n];
            if (intersectRect(button, actor.rect) && m_callback)
            {
                m_callback->touchButton(button.buttonId, button.buttonType);
            }
        }

        for (int n = 0; n < m_portals.size(); ++n)
        {
            Portal &portal = m_portals[n];
            if (intersectRect(portal.getPos(), actor.rect) && m_callback)
            {
                m_callback->levelWarp(portal.getTargetLevel());
            }
        }

        actor.dy += 3; // gravity
        if (actor.dy > 32) actor.dy = 32; // cap max speed of actor
        if (actor.jetpackLockoutFrames > 0)
        {
            actor.jetpackLockoutFrames -= 1;
        }
    }
}


void Physics::startActorJump(int index)
{
    Actor& actor = m_actors[index];
    if (actor.isOnGround) // normal jump
    {
        m_sound->playJumpSound();
        actor.dy = -64;
        actor.jetpackLockoutFrames = 20;
    }
    
}

void Physics::activateJetpack(int index)
{
    Actor& actor = m_actors[index];
    if (!actor.isOnGround)
    {
        if (actor.jetpackFrames > 0 && actor.jetpackLockoutFrames == 0)
        {
            actor.jetpackFrames -= 1;
            actor.dy -= JETPACK_THRUST_TABLE[JETPACK_THRUST_TABLE_SIZE - 1 - actor.jetpackFrames];
            m_jetpackActive = true;
        }
    }
}

bool Physics::jetpackIsActive()
{
    return m_lastFrameJetpackActive;
}


void Physics::activateSunPull(int index)
{
    if (m_sun.x > 0 && m_sun.y > 0)
    {
        Actor& actor = m_actors[index];
        
        // Calculate vector between actor and sun
        int32_t dx = m_sun.x * 16 - actor.rect.x;
        int32_t dy = m_sun.y * 16 - actor.rect.y;

        // calculate length of vector using 1 norm (euclidean 2 norm is too slow, as it requires a square root)
        int32_t length = labs(dx) + labs(dy);

        if (length == 0)
        {
            dx = 0;
            dy = 0;
        }
        else
        {
            dx = (dx * 16) / length;
            dy = (dy * 16) / length;
        }

        // now set vector to actor speed
        actor.dx = dx;
        actor.dy = dy;
    }
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

/**
 * Classifies which corners of r2 lie inside the rectangle given by its edges (edges included).
 *
 * A corner is inside if both its x and its y coordinate are inside, so instead of testing
 * all four corners separately, only the two x and the two y coordinates of r2 are tested.
 */
static IntersectionType classifyIntersection(int left, int top, int right, int bottom, const Rectangle &r2)
{
    // truncated to 16 bit like the corner coordinates in the original corner tests
    int16_t r2Right = r2.x + r2.width;
    int16_t r2Bottom = r2.y + r2.height;

    bool leftInside   = r2.x >= left && r2.x <= right;
    bool rightInside  = r2Right >= left && r2Right <= right;
    bool topInside    = r2.y >= top && r2.y <= bottom;
    bool bottomInside = r2Bottom >= top && r2Bottom <= bottom;

    if (leftInside && rightInside && bottomInside && !topInside)
    {
        return INTERSECTION_TOP; // only both bottom corners are inside
    }
    if (leftInside && !rightInside && topInside && bottomInside)
    {
        return INTERSECTION_RIGHT; // only both left corners are inside
    }
    if (!leftInside && rightInside && topInside && bottomInside)
    {
        return INTERSECTION_LEFT; // only both right corners are inside
    }
    if (leftInside && rightInside && topInside && !bottomInside)
    {
        return INTERSECTION_BOTTOM; // only both top corners are inside
    }

    return INTERSECTION_OTHER;
}

IntersectionType Physics::getIntersectionType(const Rectangle &r1, const Rectangle &r2)
{
    return classifyIntersection(r1.x, r1.y, r1.x + r1.width, r1.y + r1.height, r2);
}

IntersectionType Physics::getIntersectionType(const Rectangle &r1, int16_t extendHorizontal, int16_t extendVertical, const Rectangle &r2)
{
    // 16 bit variables, so the results match a Rectangle that was extended in place
    int16_t x = r1.x - extendHorizontal;
    int16_t y = r1.y - extendVertical;
    int16_t width = r1.width + extendHorizontal * 2;
    int16_t height = r1.height + extendVertical * 2;
    return classifyIntersection(x, y, x + width, y + height, r2);
}
