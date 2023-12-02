#include "unit_test.h"
#include "time_tools.h"
#include "tiny_string.h"

#include <string.h>


TEST(CreateTimeStringTest)
{
    char* result = createTimeString(0ul);
    ASSERT_TRUE(strcmp(result, "00:00:00.000") == 0);

    result = createTimeString(1ul);
    ASSERT_TRUE(strcmp(result, "00:00:00.014") == 0);

    result = createTimeString(21ul);
    ASSERT_TRUE(strcmp(result, "00:00:00.300") == 0);

    result = createTimeString(70ul);
    ASSERT_TRUE(strcmp(result, "00:00:01.000") == 0);

    result = createTimeString(70ul * 60ul);
    ASSERT_TRUE(strcmp(result, "00:01:00.000") == 0);

    result = createTimeString(70ul * 61ul);
    ASSERT_TRUE(strcmp(result, "00:01:01.000") == 0);

    result = createTimeString(70ul * 60ul * 60ul);
    ASSERT_TRUE(strcmp(result, "01:00:00.000") == 0);

    result = createTimeString(70ul * 60ul * 60ul * 99ul);
    ASSERT_TRUE(strcmp(result, "99:00:00.000") == 0);

    result = createTimeString(70ul * 60ul * 60ul * 100ul);
    ASSERT_TRUE(strcmp(result, "100:00:00.000") == 0);

    result = createTimeString(70ul * 60ul * 60ul * 1000ul);
    ASSERT_TRUE(strcmp(result, "1000:00:00.000") == 0);

    result = createTimeString(70ul * 60ul * 60ul * 1000ul + 70ul);
    ASSERT_TRUE(strcmp(result, "1000:00:01.000") == 0);
}

