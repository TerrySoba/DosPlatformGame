#include "unit_test.h"
#include "i18n.h"

#include "test_data.h"

TEST(I18NTest)
{
    I18N::loadTranslations(TEST_DATA_DIR "strings.en");

    TinyString str = I18N::getString(12345);
    ASSERT_TRUE(str == "This is a test");

    str = I18N::getString(12346);
    ASSERT_TRUE(str == "??? id:12346");
}
