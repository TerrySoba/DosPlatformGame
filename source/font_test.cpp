#include "unit_test.h"
#include "exception.h"
#include "font.h"

#include "test_data.h"

TEST(FontTest1)
{
    Font font(TEST_DATA_DIR "ubuntu10.stf");
    const Character* characters = font.getCharacters();
    int numCharacters = font.getNumCharacters();
    ASSERT_EQ_INT(numCharacters, 102);
    ASSERT_EQ_INT(characters[0].character, ' ');
    ASSERT_EQ_INT(characters[1].character, '-');
    ASSERT_EQ_INT(characters[101].character, '}');
    ASSERT_EQ_INT(font.width(), 68);
    ASSERT_EQ_INT(font.height(), 68);
    ASSERT_TRUE(font.data() != NULL);
}


TEST(FontTestExceptions)
{
    ASSERT_THROW(
        Font font("fonts\\nonexistent.stf")
    , Exception);
}