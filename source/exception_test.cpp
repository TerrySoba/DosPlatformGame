#include "unit_test.h"

#include "exception.h"
#include "string.h"

TEST(ExceptionTest1)
{
    try
    {
        THROW_EXCEPTION("Test exception");
        ASSERT_TRUE(false);
    }
    catch(const Exception& e)
    {
        ASSERT_TRUE(strstr(e.what(), "Test exception") != NULL);
    }
}

TEST(ExceptionTestConcat)
{
    try
    {
        THROW_EXCEPTION("Test exception", " with concatenation");
        ASSERT_TRUE(false);
    }
    catch(const Exception& e)
    {
        ASSERT_TRUE(strstr(e.what(), "Test exception with concatenation") != NULL);
    }
}

// The test is disabled on DOS because on dos the filename is not included in
// the exception message to safe resources
#ifndef __WATCOMC__
TEST(ExceptionTestFilename)
{
    try
    {
        THROW_EXCEPTION("Filename test");
        ASSERT_TRUE(false);
    }
    catch(const Exception& e)
    {
        ASSERT_TRUE(strstr(e.what(), "file:") != NULL);
        ASSERT_TRUE(strstr(e.what(), "exception_test.cpp") != NULL);
        ASSERT_TRUE(strstr(e.what(), "line:") != NULL);
    }
}
#endif

