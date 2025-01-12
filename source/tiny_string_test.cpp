#include "unit_test.h"
#include "tiny_string.h"

#include <string.h>
#include <stdio.h>

TEST(StringConcatTest)
{
    TinyString str1 = "Hello";
    TinyString str2 = "World";
    ASSERT_TRUE(str1 + str2 == "HelloWorld");
    ASSERT_TRUE(str1 + "Game" == "HelloGame");
    ASSERT_TRUE("Nice" + str2 == "NiceWorld");
    ASSERT_TRUE(str1 + "" == "Hello");
    ASSERT_TRUE("" + str2 == "World");
}


TEST(IntToStringTest)
{
    char buffer[32];
    intToString(0, 10, buffer, 32);
    ASSERT_TRUE(strcmp(buffer, "0") == 0);

    intToString(1, 10, buffer, 32);
    ASSERT_TRUE(strcmp(buffer, "1") == 0);

    intToString(10, 10, buffer, 32);
    ASSERT_TRUE(strcmp(buffer, "10") == 0);

    intToString(123, 10, buffer, 32);
    ASSERT_TRUE(strcmp(buffer, "123") == 0);

    intToString(1234, 10, buffer, 32);
    ASSERT_TRUE(strcmp(buffer, "1234") == 0);

    intToString(12345, 10, buffer, 32);
    ASSERT_TRUE(strcmp(buffer, "12345") == 0);

    intToString(123456, 10, buffer, 32);
    ASSERT_TRUE(strcmp(buffer, "123456") == 0);

    intToString(1234567, 10, buffer, 32);
    ASSERT_TRUE(strcmp(buffer, "1234567") == 0);

    intToString(12345678, 10, buffer, 32);
    ASSERT_TRUE(strcmp(buffer, "12345678") == 0);

    intToString(123456789, 10, buffer, 32);
    ASSERT_TRUE(strcmp(buffer, "123456789") == 0);

    intToString(1234567890, 10, buffer, 32);
    ASSERT_TRUE(strcmp(buffer, "1234567890") == 0);


    // now test minimum length
    intToString(0, 16, buffer, 32, 5, ' ');
    ASSERT_TRUE(strcmp(buffer, "    0") == 0);

    intToString(0xab, 16, buffer, 32, 5, '0');
    ASSERT_TRUE(strcmp(buffer, "000AB") == 0);

    intToString(0x12345, 16, buffer, 32, 1, ' ');
    ASSERT_TRUE(strcmp(buffer, "12345") == 0);


    // now test negative numbers
    intToString(-1, 10, buffer, 32);
    ASSERT_TRUE(strcmp(buffer, "-1") == 0);

    intToString(-10, 10, buffer, 32);
    ASSERT_TRUE(strcmp(buffer, "-10") == 0);



    ////// test return value
    // buffer too small
    ASSERT_TRUE(intToString(0, 10, buffer, 0) == 0);
    ASSERT_TRUE(intToString(0, 10, buffer, 1) == 0);

    // buffer just big enough
    ASSERT_TRUE(intToString(0, 10, buffer, 2) == 1);
    ASSERT_TRUE(intToString(9, 10, buffer, 2) == 1);

    // buffer bigger that needed
    ASSERT_TRUE(intToString(2, 10, buffer, 3) == 1);

    // some random cases
    ASSERT_TRUE(intToString(123, 10, buffer, 10) == 3);
    ASSERT_TRUE(intToString(-123, 10, buffer, 10) == 4);
    ASSERT_TRUE(intToString(0xabc, 16, buffer, 10) == 3);
    ASSERT_TRUE(intToString(0xabc, 16, buffer, 10, 5, '0') == 5);
    ASSERT_TRUE(intToString(0xabc, 16, buffer, 10, 5, ' ') == 5);
    ASSERT_TRUE(intToString(1234, 10, buffer, 10, 5, ' ') == 5);
    ASSERT_TRUE(intToString(1234, 10, buffer, 10, 5, '0') == 5);
}


TEST(TinyStringOperatorTest)
{
    TinyString str1 = "Hello";
    TinyString str2 = "World";
    ASSERT_TRUE(str1 == "Hello");
    ASSERT_TRUE(str2 == "World");
    ASSERT_TRUE(str1 != "World");
    ASSERT_TRUE(str2 != "Hello");
    ASSERT_TRUE(str1 == str1);
    ASSERT_TRUE(str2 == str2);
    ASSERT_TRUE(str1 != str2);
    ASSERT_TRUE(str2 != str1);
    ASSERT_FALSE(str1 != str1);
    ASSERT_FALSE(str2 != str2);

    ASSERT_TRUE(str1[0] == 'H');
    ASSERT_TRUE(str1[1] == 'e');
    ASSERT_TRUE(str1[2] == 'l');
    ASSERT_TRUE(str1[3] == 'l');
    ASSERT_TRUE(str1[4] == 'o');
    ASSERT_TRUE(str1[5] == 0);
}
