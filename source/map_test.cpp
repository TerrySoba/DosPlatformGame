#include "unit_test.h"
#include "map.h"

TEST(MapTestInt)
{
    tnd::map<int, int> m;
    m[7] = 10;
    m[1] = 20;
    m[2] = 30;
    m[4] = 40;
    m[5] = 50;
    m[6] = 60;
    m[3] = 70;
    
    ASSERT_TRUE(m[7] == 10);
    ASSERT_TRUE(m[1] == 20);
    ASSERT_TRUE(m[2] == 30);
    ASSERT_TRUE(m[4] == 40);
    ASSERT_TRUE(m[5] == 50);
    ASSERT_TRUE(m[6] == 60);
    ASSERT_TRUE(m[3] == 70);

    tnd::map<int, int>::iterator it = m.begin();
    ASSERT_TRUE(it->first == 1);
    ASSERT_TRUE(it->second == 20);
    ++it;
    ASSERT_TRUE(it->first == 2);
    ASSERT_TRUE(it->second == 30);
    ++it;
    ASSERT_TRUE(it->first == 3);
    ASSERT_TRUE(it->second == 70);
    ++it;
    ASSERT_TRUE(it->first == 4);
    ASSERT_TRUE(it->second == 40);
    ++it;
    ASSERT_TRUE(it->first == 5);
    ASSERT_TRUE(it->second == 50);
    ++it;
    ASSERT_TRUE(it->first == 6);
    ASSERT_TRUE(it->second == 60);
    ++it;
    ASSERT_TRUE(it->first == 7);
    ASSERT_TRUE(it->second == 10);
    ++it;
    ASSERT_TRUE(it == m.end());
}
