#include "unit_test.h"
#include "credits_loader.h"

#include "test_data.h"

TEST(CreditsLoaderTest)
{
    CreditsLoader loader(TEST_DATA_DIR "credits.txt");
    ASSERT_EQ_INT(loader.size(), 14);
    ASSERT_EQ_INT(loader.getEntry(0).type, CREDITS_TYPE_ROLE);
    ASSERT_TRUE(loader.getEntry(0).text == "- Programming -");
    ASSERT_EQ_INT(loader.getEntry(1).type, CREDITS_TYPE_PERSON);
    ASSERT_TRUE(loader.getEntry(1).text == "Torsten Stremlau");
    ASSERT_EQ_INT(loader.getEntry(2).type, CREDITS_TYPE_SECTION_END);
    ASSERT_TRUE(loader.getEntry(2).text == "");
}