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