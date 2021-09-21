#include "unit_test.h"
#include "animation.h"

TEST(AnimationTest)
{
    Animation ani("images\\guy.jsn", "images\\guy.tga");
    tnd::vector<FrameTag> tags = ani.getTags();
    for (int i = 0; i < tags.size(); ++i)
    {
        std::cout << "Tag: " << tags[i].name.c_str() << " " << tags[i].startFrame << "->" << tags[i].endFrame << "\n";
    }
}
