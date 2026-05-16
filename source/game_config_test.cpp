#include "unit_test.h"

#include "game_config.h"

#include "exception.h"

TEST(GameConfigTestGood)
{
    GameConfig config = readGameConfig(TEST_DATA_DIR "config.ini");

    ASSERT_EQ_INT(config.upKey, 0x4b);
    ASSERT_EQ_INT(config.downKey, 0x4d);
    ASSERT_EQ_INT(config.leftKey, 0x48);
    ASSERT_EQ_INT(config.rightKey, 0x50);
    ASSERT_EQ_INT(config.action1Key, 0x38);
    ASSERT_EQ_INT(config.action2Key, 0x39);
    ASSERT_EQ_INT(config.action1JoyButton, 0);
    ASSERT_EQ_INT(config.action2JoyButton, 1);
}

TEST(GameConfigTestException)
{
    ASSERT_THROW(readGameConfig(TEST_DATA_DIR "no_file.ini"), Exception);
}
