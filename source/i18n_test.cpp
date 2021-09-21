#include "unit_test.h"
#include "i18n.h"

TEST(ZI18NTest)
{

    I18N::loadTranslations("strings.en");

    TinyString str = I18N::getString(100);
    ASSERT_TRUE(str == "This is yet another sample text.");

    str = I18N::getString(1);
    ASSERT_TRUE(str == "\xE4\xF6\xFC\xDF");

    str = I18N::getString(123);
    ASSERT_TRUE(str == "???");
}
