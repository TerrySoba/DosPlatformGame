#include "unit_test.h"

#include <stdint.h>
#include <string.h>

extern "C"
{
    extern void drawRleSprite(char* spriteData, char* dst, uint16_t lineWidth);
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



void drawRleSpriteC(char* spriteData, char* dst, uint16_t lineWidth)
{
    uint8_t* lastLineStart = (uint8_t*)dst; // store the start of the current line
    for (uint16_t y = 0; y < lineWidth; y++)
    {
        uint8_t rleType = *spriteData & 0xC0;
        uint8_t length = *spriteData & 0x3F;
        spriteData++;

        switch (rleType)
        {
            case RLE_TYPE_CONSECUTIVE:
                memcpy(dst, spriteData, length);
                dst += length;
                spriteData += length;
                break;

            case RLE_TYPE_REPEAT:
                memset(dst, *spriteData, length);
                dst += length;
                spriteData++;
                break;

            case RLE_TYPE_SKIP:
                if (length == 0)
                {
                    dst = (char*)lastLineStart + lineWidth;  // skip to the end of the line
                    lastLineStart = (uint8_t*)dst; // update the lastLineStart to the new line
                }
                else
                {
                    dst += length;
                }
                
                break;

            case RLE_TYPE_END:
                return; // return from the function
        }
    }
}



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