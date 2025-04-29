#include "unit_test.h"
#include "tga_image.h"

TEST(TgaImageTest)
{
    TgaImage tgaImage1(TEST_DATA_DIR "gradient.tga");

    ASSERT_TRUE(tgaImage1.width() == 320);
    ASSERT_TRUE(tgaImage1.height() == 200);

    ASSERT_TRUE(tgaImage1.data() != 0);

    ASSERT_TRUE(tgaImage1.data()[0] == 0);
    ASSERT_TRUE(tgaImage1.data()[1] == 0);
    ASSERT_TRUE(tgaImage1.data()[2] == 0);

    TgaImage tgaImage2(TEST_DATA_DIR "gradcomp.tga");

    ASSERT_TRUE(tgaImage2.width() == 320);
    ASSERT_TRUE(tgaImage2.height() == 200);
}
