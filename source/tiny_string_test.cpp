#include "unit_test.h"
#include "tiny_string.h"

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
