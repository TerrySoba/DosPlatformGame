#include "unit_test.h"
#include "level.h"


TEST(LevelTest1)
{
    Level level(TEST_DATA_DIR "0101.map", 16, 16, -8, -8);
    // printf("w:%d h:%d\n", level.width(), level.height());

    tnd::vector<Rectangle> death = level.getDeath();
    // printf("death.size() == %d\n", death.size());
    // for (int i = 0; i < death.size(); ++i)
    // {
    //     printf("lala\n");
    // }
}
