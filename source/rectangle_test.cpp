#include "unit_test.h"
#include "rectangle.h"

TEST(RectangleIntersection)
{
    {
        Rectangle a(0, 0, 100, 100);
        Rectangle b(-100, -100, 150, 120);
        Rectangle inter1 = a.intersection(b);
        Rectangle inter2 = b.intersection(a);
        ASSERT_TRUE(inter1 == Rectangle(0,0,50,20));
        ASSERT_TRUE(inter2 == inter1);
        ASSERT_FALSE(inter1.empty());
    }

    {
        Rectangle a(0, 0, 100, 100);
        Rectangle b(50, 50, 10, 20);
        Rectangle inter1 = a.intersection(b);
        Rectangle inter2 = b.intersection(a);
        ASSERT_TRUE(inter1 == Rectangle(50,50,10,20));
        ASSERT_TRUE(inter2 == inter1);
        ASSERT_FALSE(inter1.empty());
    }

    {
        Rectangle a(1000, 1000, 100, 100);
        Rectangle b(50, 50, 10, 20);
        Rectangle inter1 = a.intersection(b);
        Rectangle inter2 = b.intersection(a);
        ASSERT_TRUE(inter1 == Rectangle(0,0,0,0));
        ASSERT_TRUE(inter2 == inter1);
        ASSERT_TRUE(inter1.empty());
    }

    {
        Rectangle a(0, 0, 320, 200);
        Rectangle b(0, 16, 16, 16);
        Rectangle inter1 = a.intersection(b);
        Rectangle inter2 = b.intersection(a);

        // std::cout << "inter1: x:" << inter1.x << " y:" << inter1.y << " w:" << inter1.width << " h:" << inter1.height << "\n";
        // std::cout << "inter2: x:" << inter2.x << " y:" << inter2.y << " w:" << inter2.width << " h:" << inter2.height << "\n";

        ASSERT_TRUE(inter2 == inter1);
        ASSERT_TRUE(inter1 == Rectangle(0,16,16,16));
        ASSERT_FALSE(inter1.empty());
    }
}
