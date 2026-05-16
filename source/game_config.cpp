#include "game_config.h"

#include "exception.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

namespace
{
enum ConfigSection
{
    SECTION_NONE,
    SECTION_KEYBOARD,
    SECTION_JOYSTICK
};

char* trimLeft(char* text)
{
    while (*text == ' ' || *text == '\t' || *text == '\r' || *text == '\n') {
        ++text;
    }
    return text;
}

void trimRightInPlace(char* text)
{
    size_t len = strlen(text);
    while (len > 0) {
        const char c = text[len - 1];
        if (c != ' ' && c != '\t' && c != '\r' && c != '\n') {
            break;
        }
        text[len - 1] = '\0';
        --len;
    }
}

bool equalsIgnoreCase(const char* a, const char* b)
{
    for (;;) {
        char ca = *a;
        char cb = *b;

        if (ca >= 'a' && ca <= 'z') {
            ca = static_cast<char>(ca - ('a' - 'A'));
        }
        if (cb >= 'a' && cb <= 'z') {
            cb = static_cast<char>(cb - ('a' - 'A'));
        }

        if (ca != cb) {
            return false;
        }
        if (ca == '\0') {
            return true;
        }

        ++a;
        ++b;
    }
}

uint8_t parseU8(const char* value)
{
    char* end = NULL;
    const unsigned long parsedValue = strtoul(value, &end, 0);
    if (end == value || *trimLeft(end) != '\0' || parsedValue > 0xffUL) {
        THROW_EXCEPTION("Invalid config value:", value);
    }
    return static_cast<uint8_t>(parsedValue);
}
} // namespace

GameConfig readGameConfig(const char* filename)
{
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        THROW_EXCEPTION("Could not open file:", filename);
    }

    GameConfig config = {0};
    ConfigSection section = SECTION_NONE;
    char lineBuffer[80];

    while (fgets(lineBuffer, sizeof(lineBuffer), fp)) {
        trimRightInPlace(lineBuffer);

        char* line = trimLeft(lineBuffer);
        if (*line == '\0' || *line == ';' || *line == '#') {
            continue;
        }

        if (*line == '[') {
            char* sectionEnd = strchr(line + 1, ']');
            if (!sectionEnd) {
                fclose(fp);
                THROW_EXCEPTION("Invalid section in config:", filename);
            }

            *sectionEnd = '\0';
            char* sectionName = trimLeft(line + 1);
            trimRightInPlace(sectionName);

            if (equalsIgnoreCase(sectionName, "keyboard")) {
                section = SECTION_KEYBOARD;
            } else if (equalsIgnoreCase(sectionName, "joystick")) {
                section = SECTION_JOYSTICK;
            } else {
                section = SECTION_NONE;
            }
            continue;
        }

        char* equalSign = strchr(line, '=');
        if (!equalSign) {
            continue;
        }

        *equalSign = '\0';
        char* key = trimLeft(line);
        trimRightInPlace(key);

        char* value = trimLeft(equalSign + 1);
        trimRightInPlace(value);

        const uint8_t parsedValue = parseU8(value);

        if (section == SECTION_KEYBOARD) {
            if (equalsIgnoreCase(key, "UP")) {
                config.upKey = parsedValue;
            } else if (equalsIgnoreCase(key, "DOWN")) {
                config.downKey = parsedValue;
            } else if (equalsIgnoreCase(key, "LEFT")) {
                config.leftKey = parsedValue;
            } else if (equalsIgnoreCase(key, "RIGHT")) {
                config.rightKey = parsedValue;
            } else if (equalsIgnoreCase(key, "ACTION1")) {
                config.action1Key = parsedValue;
            } else if (equalsIgnoreCase(key, "ACTION2")) {
                config.action2Key = parsedValue;
            }
        } else if (section == SECTION_JOYSTICK) {
            // Keep legacy mapping expected by the current tests/config behavior.
            if (equalsIgnoreCase(key, "ACTION1")) {
                config.action1JoyButton = parsedValue;
            } else if (equalsIgnoreCase(key, "ACTION2")) {
                config.action2JoyButton = parsedValue;
            }
        }
    }

    fclose(fp);

    return config;
}

