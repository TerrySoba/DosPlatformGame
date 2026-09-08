#include "unit_test.h"

#include "game_config.h"

TEST(ConfigTest)
{
    // IniFile ini(TEST_DATA_DIR "config.ini");
    GameConfig config = parseGameConfig(TEST_DATA_DIR "config.ini");
    ASSERT_TRUE(config.enableSound == true);
    ASSERT_TRUE(config.enableMusic == false);
    ASSERT_TRUE(config.keyboard.keyUp == 75);
    ASSERT_TRUE(config.keyboard.keyDown == 0x4d);
    ASSERT_TRUE(config.keyboard.keyLeft == 0x48);
    ASSERT_TRUE(config.keyboard.keyRight == 0x50);
    ASSERT_TRUE(config.keyboard.keyJump == 0x1D);
    ASSERT_TRUE(config.keyboard.keyAction == 0x38);
}

TEST(SetIniValueTest)
{
    const char* fileName = TEST_DATA_DIR "set_test.ini";
    remove(fileName);

    // insert into a brand new file
    setIniValue(fileName, "sound", "enable_sound", "true");
    GameConfig config = parseGameConfig(fileName);
    ASSERT_TRUE(config.enableSound == true);

    // add a new key to an existing section
    setIniValue(fileName, "sound", "enable_music", "true");
    config = parseGameConfig(fileName);
    ASSERT_TRUE(config.enableSound == true);
    ASSERT_TRUE(config.enableMusic == true);

    // change an existing key's value
    setIniValue(fileName, "sound", "enable_music", "false");
    config = parseGameConfig(fileName);
    ASSERT_TRUE(config.enableMusic == false);

    // add a key into a brand new section
    setIniValue(fileName, "keyboard", "up", "75");
    config = parseGameConfig(fileName);
    ASSERT_TRUE(config.keyboard.keyUp == 75);
    ASSERT_TRUE(config.enableSound == true);
    ASSERT_TRUE(config.enableMusic == false);

    remove(fileName);
}


TEST(WriteKeyboardConfigTest)
{
    const char* fileName = TEST_DATA_DIR "keyb.ini";
    remove(fileName);

    KeyboardConfig keyboard = {1, 2, 3, 4, 5, 6};
    writeKeyboardConfig(fileName, keyboard);
    GameConfig config = parseGameConfig(fileName);
    ASSERT_TRUE(config.keyboard.keyUp == 1);
    ASSERT_TRUE(config.keyboard.keyDown == 2);
    ASSERT_TRUE(config.keyboard.keyLeft == 3);
    ASSERT_TRUE(config.keyboard.keyRight == 4);
    ASSERT_TRUE(config.keyboard.keyJump == 5);
    ASSERT_TRUE(config.keyboard.keyAction == 6);

    remove(fileName);
}