#include "unit_test.h"
#include "sprite.h"
#include "tga_image.h"
#include "test_data.h"
#include "framebuffer_gfx.h"

#include "vector.h"

#include <string.h>

namespace {

class TgaPixelSource : public PixelSource
{
public:
    TgaPixelSource(const TgaImage& image)
        : m_image(image)
    {
    }

    virtual int16_t width() const
    {
        return m_image.width();
    }

    virtual int16_t height() const
    {
        return m_image.height();
    }

    virtual char pixel(int16_t x, int16_t y) const
    {
        return m_image.data()[m_image.width() * y + x];
    }

    virtual char transparentColor() const
    {
        return 0;
    }

private:
    const TgaImage& m_image;
};

}

TEST(SpriteTestFile)
{
    TgaImage image(TEST_DATA_DIR "guyframe.tga");
    Sprite sprite(TEST_DATA_DIR "guyframe.tga", 320);
    
    tnd::vector<char> target(320 * 200);
    // fill target with 0
    memset(target.data(), 12, target.size());

    sprite.draw(target.data(), 320, 200, 0, 0);

    const char* imageData = image.data();

    for (int y = 0; y < image.height(); ++y)
    {
        for (int x = 0; x < image.width(); ++x)
        {
            char imagePixel = imageData[image.width() * y + x];
            char targetPixel = target[320 * y + x];

            if (imagePixel != 0)
            {
                // Non-transparent pixel
                ASSERT_EQ_INT(imagePixel, targetPixel);
            }
            else
            {
                // Transparent pixel
                ASSERT_EQ_INT(12, targetPixel);
            }
        }
    }

    FramebufferGfx gfx;
    gfx.clear();
    for (int i = 0; i < 10; ++i)
    {
        gfx.draw(sprite, 3 * i, 3 * i);
    }
    gfx.drawScreen();
    gfx.saveAsTgaImage("test.tga");
    gfx.clear();
    gfx.draw(sprite, 0, 0);
    gfx.drawScreen();
    gfx.saveAsTgaImage("test2.tga");
}

TEST(SpriteTestPixelSource)
{
    TgaImage image(TEST_DATA_DIR "guyframe.tga");
    TgaPixelSource pixels(image);
    Sprite sprite(pixels, 320);
    
    tnd::vector<char> target(320 * 200);
    // fill target with 0
    memset(target.data(), 12, target.size());

    sprite.draw(target.data(), 320, 200, 0, 0);

    const char* imageData = image.data();

    for (int y = 0; y < image.height(); ++y)
    {
        for (int x = 0; x < image.width(); ++x)
        {
            char imagePixel = imageData[image.width() * y + x];
            char targetPixel = target[320 * y + x];

            if (imagePixel != 0)
            {
                // Non-transparent pixel
                ASSERT_EQ_INT(imagePixel, targetPixel);
            }
            else
            {
                // Transparent pixel
                ASSERT_EQ_INT(12, targetPixel);
            }
        }
    }
}

