#include "unit_test.h"

#include "level.h"

TEST(LevelTest1)
{
    shared_ptr<ImageBase> tilesImage;
    Level level("levels\\0101.map", tilesImage, 16, 16, -8, -8);

    printf("w:%d h:%d\n", level.width(), level.height());

    tnd::vector<Rectangle> death = level.getDeath();
    printf("death.size() == %d\n", death.size());
    for (int i = 0; i < death.size(); ++i)
    {
        printf("lala\n");
    }
}
