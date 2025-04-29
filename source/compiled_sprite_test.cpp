#include "unit_test.h"

#include "platform/dos/compiled_sprite.h"
#include "tga_image.h"

#include <string.h>
#include <stdio.h>

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


uint32_t countZeroes(const char* data, uint32_t size)
{
    uint32_t count = 0;
    for (uint32_t i = 0; i < size; ++i)
    {
        if (data[i] == 0)
        {
            ++count;
        }
    }
    return count;
}


uint32_t getCurrentTime()
{
#ifdef __WATCOMC__
    // use bios rtc to get current time
    uint16_t timeLow;
    uint16_t timeHigh;
    __asm
    {
        push ax
        push cx
        push dx
        mov ah, 00h 
        int 1Ah
        mov timeHigh, cx
        mov timeLow, dx
        pop dx
        pop cx
        pop ax
    }
    return ((uint32_t)timeHigh) << 16 | timeLow;
#else
    // use clock() to get current time
    return (uint32_t)clock();
#endif
}


const uint32_t TARGET_WIDTH = 320;
const uint32_t TARGET_HEIGHT = 20;
const uint32_t TARGET_SIZE = TARGET_WIDTH * TARGET_HEIGHT;

TEST(CompiledSpriteTest)
{
    TgaImage image(TEST_DATA_DIR "guyframe.tga");
    TgaPixelSource pixels(image);

    uint32_t spriteSize = compileData(NULL, 0, pixels, 320);
    ASSERT_TRUE(spriteSize > 0);

    char* sprite = new char[spriteSize];
    compileData(sprite, spriteSize, pixels, 320);

    printf("spriteSize: %d\n", spriteSize);

    char* target = new char[TARGET_SIZE];
    memset(target, 0, TARGET_SIZE);
    ASSERT_TRUE(countZeroes(target, TARGET_SIZE) == TARGET_SIZE);

    DrawCompiledSpritePtr draw = (DrawCompiledSpritePtr)sprite;
    draw(target);

    // check for pixels in the target
    for (uint32_t y = 0; y < TARGET_HEIGHT; ++y)
    {
        for (uint32_t x = 0; x < TARGET_WIDTH; ++x)
        {
            if (x < pixels.width() && y < pixels.height())
            {
                ASSERT_TRUE(target[TARGET_WIDTH * y + x] == pixels.pixel(x, y));
            }
        }
    }

    delete[] target;
    delete[] sprite;
}


// TEST(CompiledSpriteBenchmark)
// {
//    TgaImage image("images\\guyframe.tga");
//     TgaPixelSource pixels(image);

//     uint32_t spriteSize = compileData(NULL, 0, pixels, 320);
//     ASSERT_TRUE(spriteSize > 0);

//     char* sprite = new char[spriteSize];
//     compileData(sprite, spriteSize, pixels, 320);

//     printf("spriteSize: %d\n", spriteSize);

//     char* target = new char[TARGET_SIZE];
//     memset(target, 0, TARGET_SIZE);


//     DrawCompiledSpritePtr draw = (DrawCompiledSpritePtr)sprite;

//     uint32_t start = getCurrentTime();
//     for (uint32_t i = 0; i < 100000; ++i)
//     {
//         draw(target);
//     }

//     uint32_t end = getCurrentTime();

//     printf("time: %lu\n", end - start);


//     delete[] target;
//     delete[] sprite;

//     ASSERT_TRUE(false);
// }