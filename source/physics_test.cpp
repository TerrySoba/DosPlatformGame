#include "unit_test.h"
#include "physics.h"
#include "sound_controller_null.h"

TEST(RectCollision)
{
    Rectangle a(  0,   0, 10, 10);
    Rectangle b(100, 100, 10, 10);
    Rectangle c(  9,   9, 10, 10);
    Rectangle d(  5,   5,  1,  1);
    Rectangle e( -5,  -5, 10, 10);
    Rectangle f(-11, -11, 10, 10);

    ASSERT_TRUE(Physics::intersectRect(a,a));
    ASSERT_FALSE(Physics::intersectRect(a,b));
    ASSERT_TRUE(Physics::intersectRect(a,c));
    ASSERT_TRUE(Physics::intersectRect(a,d));
    ASSERT_TRUE(Physics::intersectRect(a,e));
    ASSERT_FALSE(Physics::intersectRect(a,f));
}


TEST(getIntersectionTypeTest)
{
    Rectangle ref   (100, 100, 100, 100);
    Rectangle top   (110,  95,  10,  10);
    Rectangle right (150, 110, 100,  10);
    Rectangle left  ( 90, 110, 100,  10);
    Rectangle bottom(110, 195,  10,  10);

    ASSERT_TRUE(Physics::getIntersectionType(ref, top)    == INTERSECTION_TOP);
    ASSERT_TRUE(Physics::getIntersectionType(ref, right)  == INTERSECTION_RIGHT);
    ASSERT_TRUE(Physics::getIntersectionType(ref, left)   == INTERSECTION_LEFT);
    ASSERT_TRUE(Physics::getIntersectionType(ref, bottom) == INTERSECTION_BOTTOM);
}

class CallBackTest : public PhysicsCallback
{
    void levelTransition(LevelTransition transition) {}
};

// Actor size and positions are in subpixels
static const int16_t TEST_ACTOR_WIDTH = 256;
static const int16_t TEST_ACTOR_HEIGHT = 384;

static Actor createTestActor(int16_t x, int16_t y)
{
    Actor actor;
    actor.rect = Rectangle(x, y, TEST_ACTOR_WIDTH, TEST_ACTOR_HEIGHT);
    return actor;
}

static void runPhysicsFrames(Physics& physics, int frames)
{
    for (int i = 0; i < frames; ++i)
    {
        physics.calc();
    }
}

TEST(FallThroughPlatformLandsOnSolidFloor)
{
    Physics physics(0, tnd::shared_ptr<SoundController>(new SoundControllerNull()));

    Rectangle platform(0, 1000, 2000, 216);
    Rectangle floor(0, 3000, 2000, 256);

    tnd::vector<Rectangle> walls;
    walls.push_back(platform);
    walls.push_back(floor);
    tnd::vector<Rectangle> fallThrough;
    fallThrough.push_back(platform);
    physics.setWalls(walls);
    physics.setFallThrough(fallThrough);

    physics.addActor(createTestActor(100, platform.y - TEST_ACTOR_HEIGHT));
    physics.setActorDuck(0, true);

    runPhysicsFrames(physics, 300);

    ASSERT_EQ_INT(physics.m_actors[0].rect.y, floor.y - TEST_ACTOR_HEIGHT);
    ASSERT_TRUE(physics.m_actors[0].isOnGround);
}

TEST(DuckingDoesNotFallThroughAdjacentSolidWall)
{
    Physics physics(0, tnd::shared_ptr<SoundController>(new SoundControllerNull()));

    // actor stands on both, a fall through platform and a solid wall right next to it
    Rectangle platform(0, 1000, 1000, 216);
    Rectangle solid(1000, 1000, 1000, 216);

    tnd::vector<Rectangle> walls;
    walls.push_back(platform);
    walls.push_back(solid);
    tnd::vector<Rectangle> fallThrough;
    fallThrough.push_back(platform);
    physics.setWalls(walls);
    physics.setFallThrough(fallThrough);

    physics.addActor(createTestActor(900, platform.y - TEST_ACTOR_HEIGHT));
    physics.setActorDuck(0, true);

    runPhysicsFrames(physics, 50);

    ASSERT_EQ_INT(physics.m_actors[0].rect.y, solid.y - TEST_ACTOR_HEIGHT);
    ASSERT_TRUE(physics.m_actors[0].isOnGround);

    // after getting up again, walking onto the platform must not fall through it
    physics.setActorDuck(0, false);
    runPhysicsFrames(physics, 2);
    physics.m_actors[0].rect.x = 100;
    runPhysicsFrames(physics, 50);

    ASSERT_EQ_INT(physics.m_actors[0].rect.y, platform.y - TEST_ACTOR_HEIGHT);
    ASSERT_TRUE(physics.m_actors[0].isOnGround);
}

TEST(DeathWhileFallingThroughResetsFallThrough)
{
    Physics physics(0, tnd::shared_ptr<SoundController>(new SoundControllerNull()));

    Rectangle spawnPlatform(0, 1000, 2000, 216);
    Rectangle lowerPlatform(0, 2100, 2000, 216);

    tnd::vector<Rectangle> walls;
    walls.push_back(spawnPlatform);
    walls.push_back(lowerPlatform);
    tnd::vector<Rectangle> fallThrough;
    fallThrough.push_back(spawnPlatform);
    fallThrough.push_back(lowerPlatform);
    physics.setWalls(walls);
    physics.setFallThrough(fallThrough);

    tnd::vector<Rectangle> death;
    death.push_back(Rectangle(0, 1950, 2000, 100));
    physics.setDeath(death);

    // actor is in the middle of falling through the lower platform when it dies
    Actor actor = createTestActor(100, 2000);
    actor.isFallingThrough = true;
    physics.addActor(actor);
    physics.setSpawnPoint(Point(100, spawnPlatform.y - TEST_ACTOR_HEIGHT));
    physics.m_actors[0].rect.y = 2000;

    // first frame kills the actor, afterwards it must stand on the spawn platform right away
    runPhysicsFrames(physics, 5);

    ASSERT_EQ_INT(physics.m_actors[0].rect.y, spawnPlatform.y - TEST_ACTOR_HEIGHT);
    ASSERT_TRUE(physics.m_actors[0].isOnGround);
}

// Reference implementations of the collision code before it was optimized,
// used to make sure that the optimized versions give identical results.
static bool referencePointIntersect(const Rectangle& r, int16_t x, int16_t y)
{
    return (x >= r.x && x <= r.x + r.width) &&
           (y >= r.y && y <= r.y + r.height);
}

static IntersectionType referenceIntersectionType(const Rectangle &r1, const Rectangle &r2)
{
    bool topLeft = referencePointIntersect(r1, r2.x, r2.y);
    bool topRight = referencePointIntersect(r1, r2.x + r2.width, r2.y);
    bool bottomLeft = referencePointIntersect(r1, r2.x, r2.y + r2.height);
    bool bottomRight = referencePointIntersect(r1, r2.x + r2.width, r2.y + r2.height);

    if (bottomLeft && bottomRight && !topLeft && !topRight) return INTERSECTION_TOP;
    if (bottomLeft && !bottomRight && topLeft && !topRight) return INTERSECTION_RIGHT;
    if (!bottomLeft && bottomRight && !topLeft && topRight) return INTERSECTION_LEFT;
    if (!bottomLeft && !bottomRight && topLeft && topRight) return INTERSECTION_BOTTOM;
    return INTERSECTION_OTHER;
}

static Rectangle referenceExtendRectangle(Rectangle rect, int16_t horizontal, int16_t vertical)
{
    rect.x -= horizontal;
    rect.width += horizontal * 2;
    rect.y -= vertical;
    rect.height += vertical * 2;
    return rect;
}

// small deterministic pseudo random generator, so that failures are reproducible
static uint16_t s_testRandomState = 12345;
static int16_t testRandom(int16_t min, int16_t max)
{
    s_testRandomState = s_testRandomState * 25173u + 13849u;
    return min + (int16_t)(s_testRandomState % (uint16_t)(max - min + 1));
}

static Rectangle randomTestRect()
{
    // small area, so that a good share of the rectangles intersect or touch
    return Rectangle(testRandom(-200, 600), testRandom(-200, 600), testRandom(0, 400), testRandom(0, 400));
}

TEST(GetIntersectionTypeMatchesReference)
{
    for (int i = 0; i < 3000; ++i)
    {
        Rectangle r1 = randomTestRect();
        Rectangle r2 = randomTestRect();
        int16_t extendHorizontal = testRandom(-32, 300);
        int16_t extendVertical = testRandom(-32, 300);

        ASSERT_EQ_INT(Physics::getIntersectionType(r1, r2), referenceIntersectionType(r1, r2));
        ASSERT_EQ_INT(Physics::getIntersectionType(r1, extendHorizontal, extendVertical, r2),
                      referenceIntersectionType(referenceExtendRectangle(r1, extendHorizontal, extendVertical), r2));
    }

    // exact touching edges and corners
    Rectangle ref(100, 100, 100, 100);
    for (int16_t x = 80; x <= 220; x += 10)
    {
        for (int16_t y = 80; y <= 220; y += 10)
        {
            Rectangle r(x, y, 20, 20);
            ASSERT_EQ_INT(Physics::getIntersectionType(ref, r), referenceIntersectionType(ref, r));
        }
    }
}

TEST(FindIntersectingRectMatchesIntersectRect)
{
    const int rectCount = 40;
    Rectangle rects[rectCount];

    for (int round = 0; round < 50; ++round)
    {
        for (int i = 0; i < rectCount; ++i)
        {
            rects[i] = randomTestRect();
        }
        Rectangle probe = randomTestRect();
        RectBounds bounds(probe);

        // walking through all hits must find exactly the rectangles intersectRect reports
        int expected = 0;
        for (int i = 0; i < rectCount; ++i)
        {
            if (Physics::intersectRect(rects[i], probe))
            {
                ASSERT_EQ_INT(findIntersectingRect(rects, rectCount, expected, bounds), i);
                expected = i + 1;
            }
        }
        ASSERT_EQ_INT(findIntersectingRect(rects, rectCount, expected, bounds), -1);
    }

    // empty ranges
    ASSERT_EQ_INT(findIntersectingRect(rects, 0, 0, RectBounds(rects[0])), -1);
    ASSERT_EQ_INT(findIntersectingRect(rects, rectCount, rectCount, RectBounds(rects[0])), -1);
}
