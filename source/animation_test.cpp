#include "unit_test.h"
#include "animation.h"
#include "tga_image.h"

TEST(AnimationTest)
{
    Animation ani("gfx\\guy.ani", "gfx\\guy.tga");
    tnd::vector<FrameTag> tags = ani.getTags();
    for (int i = 0; i < tags.size(); ++i)
    {
        std::cout << "Tag: " << tags[i].name.c_str() << " " << tags[i].startFrame << "->" << tags[i].endFrame << "\n";
    }
}

TEST(AnimationCollisionMask)
{
    Animation ani("gfx\\enemy.ani", "gfx\\enemy.tga");

    // no masks before they are created
    ASSERT_TRUE(ani.currentCollisionMask().rows == 0);

    ani.createCollisionMasks();
    CollisionMask mask = ani.currentCollisionMask();
    ASSERT_TRUE(mask.rows != 0);
    ASSERT_EQ_INT(mask.width, 8);
    ASSERT_EQ_INT(mask.height, 15);

    // The first frame is at the top left of the sprite sheet, color 0 is transparent.
    TgaImage image("gfx\\enemy.tga");
    for (int16_t y = 0; y < mask.height; ++y)
    {
        uint16_t expected = 0;
        for (int16_t x = 0; x < mask.width; ++x)
        {
            if (image.data()[y * image.width() + x] != 0)
            {
                expected |= 0x8000u >> x;
            }
        }
        ASSERT_EQ_INT(mask.rows[y], expected);
    }
}
