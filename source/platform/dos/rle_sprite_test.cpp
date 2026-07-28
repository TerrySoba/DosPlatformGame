#include "unit_test.h"

#include <stdint.h>
#include <string.h>

extern "C"
{
    extern uint16_t drawRleSprite(char* spriteData, char* dst);
}


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

enum RleType
{
    RLE_TYPE_CONSECUTIVE = 0 << 6,
    RLE_TYPE_REPEAT = 1 << 6,
    RLE_TYPE_SKIP = 2 << 6,
    RLE_TYPE_END = 3 << 6
};



TEST(MyAsmTest)
{
    char testData[7] = {
        RLE_TYPE_SKIP | 0x02,
        RLE_TYPE_CONSECUTIVE | 0x02,
            0x04, 0x05,
        RLE_TYPE_REPEAT | 0x02,
            0x06,
        RLE_TYPE_END };

    // char testData[7] = {
    //     RLE_TYPE_CONSECUTIVE | 0x02,
    //         0x04, 0x05,
    //     RLE_TYPE_END };
            
    char expectedOutput[6] = {
        123, 123, 0x04, 0x05, 0x06, 0x06
    };

    // char* testData = "\x03";


    char* dst = new char[100]; // allocate some memory for the destination
    memset(dst, 123, 100); // initialize the destination memory to zero
    uint16_t result = drawRleSprite(testData, dst);

    ASSERT_EQ_INT(dst[0], expectedOutput[0]); // check if the result is as expected
    ASSERT_EQ_INT(dst[1], expectedOutput[1]);
    // ASSERT_EQ_INT(dst[2], expectedOutput[2]);
    // ASSERT_EQ_INT(dst[3], expectedOutput[3]);
    // ASSERT_EQ_INT(dst[4], expectedOutput[4]);
    // ASSERT_EQ_INT(dst[5], expectedOutput[5]);

    // ASSERT_TRUE(memcmp(dst, expectedOutput, 6) == 0); // compare the output with the expected output

    delete[] dst; // free the allocated memory
}