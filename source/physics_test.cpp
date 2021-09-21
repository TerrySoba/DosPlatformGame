#include "unit_test.h"
#include "physics.h"
#include "shared_ptr.h"

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
