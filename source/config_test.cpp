#include "unit_test.h"

#include "exception.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct GameConfig
{
    // sound section
    bool enableSound;
    bool enableMusic;

    // keyboard section
    uint8_t keyUp;
    uint8_t keyDown;
    uint8_t keyLeft;
    uint8_t keyRight;
    uint8_t keyJump;
    uint8_t keyAction;

    // joystick section
    uint8_t joyJump;
    uint8_t joyAction;
};

bool isspace(char c)
{
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

void parseKeyValue(const char* line, char** key, char** value)
{
    const char* p = line;
    *key = (char*)p;
    while (*p && *p != '=' && !isspace(*p)) p++;
    if (*p == '\0') return; // no '=' found
    *((char*)p) = '\0'; // terminate key
    p++;
    while (*p && (isspace(*p) || *p == '=')) p++; // skip whitespace and '=' before value
    *value = (char*)p;
    while (*p && *p != '\n' && *p != '\r') p++;
    *((char*)p) = '\0'; // terminate value
}

bool compareStrCaseInsensitive(const char* a, const char* b)
{
    while (*a && *b)
    {
        char ca = *a;
        char cb = *b;
        if (ca >= 'a' && ca <= 'z') ca -= 'a' - 'A';
        if (cb >= 'a' && cb <= 'z') cb -= 'a' - 'A';
        if (ca != cb) return false;
        a++;
        b++;
    }
    return *a == *b;
}


GameConfig parseGameConfig(const std::string& fileName)
{
    FILE* fp = fopen(fileName.c_str(), "r");
    if (!fp)
    {
        THROW_EXCEPTION("Could not open file:", fileName.c_str());
    }

    GameConfig config;

    // read the file line by line and parse the configuration
    char line[128];
    char section[16];
    section[0] = '\0'; // no section at the beginning
    while (fgets(line, sizeof(line), fp))
    {
        // remove whitespace from the line
        char* p = line;
        while (*p && isspace(*p)) p++;
        if (*p == '#' || *p == '\0' || *p == ';') continue; // skip comments and empty lines 

        printf("line: %s\n", line);

        // find key and value
        char* key = NULL;
        char* value = NULL;
        parseKeyValue(line, &key, &value);

        // if key is in '[..]' brackets, it's a section header, store the section name
        if (key && *key == '[')
        {
            char* sectionEnd = strchr(key, ']');
            if (sectionEnd)
            {
                *sectionEnd = '\0'; // terminate section name
                printf("section: '%s'\n", key + 1);
                strncpy(section, key + 1, sizeof(section) - 1);
                section[sizeof(section) - 1] = '\0';
            }
            continue;
        }

        printf("key: '%s', value: '%s'\n", key, value);
        // setGameConfigValue(config, key, value);

        if (compareStrCaseInsensitive(section, "sound"))
        {
            if (compareStrCaseInsensitive(key, "enable_sound"))
            {
                config.enableSound = (compareStrCaseInsensitive(value, "true"));
            }
            else if (compareStrCaseInsensitive(key, "enable_music"))
            {
                config.enableMusic = (compareStrCaseInsensitive(value, "true"));
            }
        }
        if (compareStrCaseInsensitive(section, "keyboard"))
        {
            if (compareStrCaseInsensitive(key, "up"))
            {
                config.keyUp = (uint8_t)atoi(value);
            }
            else if (compareStrCaseInsensitive(key, "down"))
            {
                config.keyDown = (uint8_t)atoi(value);
            }
            else if (compareStrCaseInsensitive(key, "left"))
            {
                config.keyLeft = (uint8_t)atoi(value);
            }
            else if (compareStrCaseInsensitive(key, "right"))
            {
                config.keyRight = (uint8_t)atoi(value);
            }
            else if (compareStrCaseInsensitive(key, "jump"))
            {
                config.keyJump = (uint8_t)atoi(value);
            }
            else if (compareStrCaseInsensitive(key, "action"))
            {
                config.keyAction = (uint8_t)atoi(value);
            }
        }

    }

    fclose(fp);

    return config;

}

TEST(ConfigTest)
{
    // IniFile ini(TEST_DATA_DIR "config.ini");
    GameConfig config = parseGameConfig(TEST_DATA_DIR "config.ini");
    ASSERT_TRUE(config.enableSound == true);
    ASSERT_TRUE(config.enableMusic == false);
    ASSERT_TRUE(config.keyUp == 75);
    ASSERT_TRUE(config.keyDown == 0x4d);
    ASSERT_TRUE(config.keyLeft == 0x48);
    ASSERT_TRUE(config.keyRight == 0x50);
    ASSERT_TRUE(config.keyJump == 0x1D);
    ASSERT_TRUE(config.keyAction == 0x38);
}