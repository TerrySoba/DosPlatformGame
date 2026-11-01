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
