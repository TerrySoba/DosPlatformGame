#include "unit_test.h"

#include <stdint.h>
#include <string.h>

#include "rle_sprite.h"

/*
Sprite data format:
    0x00: RLE type + length (2bits type, 6bits length)
    in case of type 0 and 1, the length is the number of pixels to draw
    0x01: pixel data ( byte)

RLE types:
    0: consecutive pixels (memcpy)
    1: same pixel repeated (memset)
    2: skip pixels (advance pointer)
    3: END of sprite data
*/

TEST(MyAsmTest)
{
    char testData[9] = {
        RLE_TYPE_SKIP | 0x02,
        RLE_TYPE_CONSECUTIVE | 0x02,
            0x04, 0x05,
        RLE_TYPE_SKIP | 0x00,
        RLE_TYPE_SKIP | 0x00,
        RLE_TYPE_REPEAT | 0x02,
            0x06,
        RLE_TYPE_END };

    // char testData[7] = {
    //     RLE_TYPE_CONSECUTIVE | 0x02,
    //         0x04, 0x05,
    //     RLE_TYPE_END };
            
    char expectedOutput[22] = {
        123, 123, 0x04, 0x05, 123, 123, 123, 123, 123, 123,
        123, 123,  123,  123, 123, 123, 123, 123, 123, 123,
        0x06, 0x06
    };

    // char* testData = "\x03";


    char* dst = new char[10 * 10]; // allocate some memory for the destination
    memset(dst, 123, 100); // initialize the destination memory to zero
    drawRleSprite(testData, dst, 10);
    // drawRleSpriteC(testData, dst, 10);

    // ASSERT_EQ_INT(result, 10); // check if the result is as expected

    for (int i = 0; i < 22; i++)
    {
        printf("dst[%d] = %d, expectedOutput[%d] = %d\n", i, dst[i], i, expectedOutput[i]);
        ASSERT_EQ_INT(dst[i], expectedOutput[i]); // compare the output with the expected output
    }

    // ASSERT_TRUE(memcmp(dst, expectedOutput, 6) == 0); // compare the output with the expected output

    delete[] dst; // free the allocated memory
}
// Straightforward reference decoder for the RLE format, used to check the assembly version.
static void referenceDrawRleSprite(const uint8_t* data, uint8_t* dst, uint16_t lineWidth)
{
    uint8_t* lineStart = dst;
    for (;;)
    {
        uint8_t type = *data & 0xC0;
        uint8_t count = *data & 0x3F;
        ++data;
        switch (type)
        {
            case RLE_TYPE_CONSECUTIVE:
                for (uint8_t i = 0; i < count; ++i) *dst++ = *data++;
                break;
            case RLE_TYPE_REPEAT:
                for (uint8_t i = 0; i < count; ++i) *dst++ = *data;
                ++data;
                break;
            case RLE_TYPE_SKIP:
                if (count == 0)
                {
                    lineStart += lineWidth;
                    dst = lineStart;
                }
                else
                {
                    dst += count;
                }
                break;
            default:
                return;
        }
    }
}

static uint16_t s_rleRandomState = 4711;
static uint16_t rleRandom(uint16_t range)
{
    s_rleRandomState = s_rleRandomState * 25173u + 13849u;
    return (s_rleRandomState >> 4) % range;
}

// Creates a random but valid RLE stream for a sprite of the given size, returns its length.
static uint16_t createRandomRleStream(uint8_t* data, uint16_t width, uint16_t height)
{
    uint16_t pos = 0;
    for (uint16_t y = 0; y < height; ++y)
    {
        uint16_t x = 0;
        // some lines are left empty, others stop early
        uint16_t lineEnd = (rleRandom(8) == 0) ? 0 : width - rleRandom(width / 2 + 1);
        while (x < lineEnd)
        {
            uint16_t maxCount = lineEnd - x;
            if (maxCount > 63) maxCount = 63;
            // mostly short runs like in real sprites, sometimes long ones
            uint16_t count = (rleRandom(4) == 0) ? 1 + rleRandom(maxCount) : 1 + rleRandom(maxCount < 4 ? maxCount : 4);
            switch (rleRandom(3))
            {
                case 0:
                    data[pos++] = RLE_TYPE_CONSECUTIVE | count;
                    for (uint16_t i = 0; i < count; ++i) data[pos++] = (uint8_t)rleRandom(256);
                    break;
                case 1:
                    data[pos++] = RLE_TYPE_REPEAT | count;
                    data[pos++] = (uint8_t)rleRandom(256);
                    break;
                default:
                    data[pos++] = RLE_TYPE_SKIP | count;
                    break;
            }
            x += count;
        }
        if (y + 1 < height)
        {
            data[pos++] = RLE_TYPE_SKIP | 0; // next line
        }
    }
    data[pos++] = (uint8_t)RLE_TYPE_END;
    return pos;
}

TEST(RleSpriteMatchesReferenceDecoder)
{
    const uint16_t maxWidth = 100;
    const uint16_t maxHeight = 20;
    const uint16_t lineWidth = 120;                          // wider than any sprite, like the screen buffer
    const uint16_t bufferSize = lineWidth * (maxHeight + 2);
    const uint16_t maxStreamSize = maxHeight * (maxWidth * 2 + 2) + 1;

    uint8_t* stream = new uint8_t[maxStreamSize];
    uint8_t* expected = new uint8_t[bufferSize];
    uint8_t* actual = new uint8_t[bufferSize];

    for (int round = 0; round < 300; ++round)
    {
        uint16_t width = 1 + rleRandom(maxWidth);
        uint16_t height = 1 + rleRandom(maxHeight);
        uint16_t length = createRandomRleStream(stream, width, height);
        ASSERT_TRUE(length <= maxStreamSize);

        // the first line is not drawn to the start of the buffer, to detect writes before the sprite
        uint16_t start = lineWidth / 2 + rleRandom(lineWidth - maxWidth);
        memset(expected, 0x5A, bufferSize);
        memset(actual, 0x5A, bufferSize);

        referenceDrawRleSprite(stream, expected + start, lineWidth);
        drawRleSprite((char*)stream, (char*)actual + start, lineWidth);

        for (uint16_t i = 0; i < bufferSize; ++i)
        {
            ASSERT_EQ_INT(actual[i], expected[i]);
        }
    }

    delete[] stream;
    delete[] expected;
    delete[] actual;
}
