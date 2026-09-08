#include "game_config.h"

#include "exception.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


bool isspace(char c)
{
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

void parseKeyValue(char* line, char** key, char** value)
{
    char* p = line;
    *key = p;
    while (*p && *p != '=' && !isspace(*p)) p++;
    if (*p == '\0') return; // no '=' found
    *p = '\0'; // terminate key
    p++;
    while (*p && (isspace(*p) || *p == '=')) p++; // skip whitespace and '=' before value
    *value = p;
    while (*p && *p != '\n' && *p != '\r') p++;
    *p = '\0'; // terminate value
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


GameConfig parseGameConfig(const char* fileName)
{
    FILE* fp = fopen(fileName, "r");
    if (!fp)
    {
        THROW_EXCEPTION("Could not open file:", fileName);
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
                strncpy(section, key + 1, sizeof(section) - 1);
                section[sizeof(section) - 1] = '\0';
            }
            continue;
        }

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
                config.keyboard.keyUp = (uint8_t)atoi(value);
            }
            else if (compareStrCaseInsensitive(key, "down"))
            {
                config.keyboard.keyDown = (uint8_t)atoi(value);
            }
            else if (compareStrCaseInsensitive(key, "left"))
            {
                config.keyboard.keyLeft = (uint8_t)atoi(value);
            }
            else if (compareStrCaseInsensitive(key, "right"))
            {
                config.keyboard.keyRight = (uint8_t)atoi(value);
            }
            else if (compareStrCaseInsensitive(key, "jump"))
            {
                config.keyboard.keyJump = (uint8_t)atoi(value);
            }
            else if (compareStrCaseInsensitive(key, "action"))
            {
                config.keyboard.keyAction = (uint8_t)atoi(value);
            }
        }

    }

    fclose(fp);

    return config;

}

// Updates key in section if present, otherwise appends key/section as needed.
// Rewrites the file line by line via a temp file, never holding the whole file in memory.
void setIniValue(const char* fileName, const char* section, const char* key, const char* value)
{
    const char* tempFileName = "temp.ini";
    FILE* out = fopen(tempFileName, "w");
    if (!out)
    {
        THROW_EXCEPTION("Could not open temp file:", tempFileName);
    }

    FILE* in = fopen(fileName, "r");

    char currentSection[32];
    currentSection[0] = '\0';
    bool inTargetSection = false; // are we currently inside the requested section?
    bool sectionFound = false;
    bool keyWritten = false;

    if (in)
    {
        char line[128];
        while (fgets(line, sizeof(line), in))
        {
            char workLine[128];
            strncpy(workLine, line, sizeof(workLine) - 1);
            workLine[sizeof(workLine) - 1] = '\0';

            char* p = workLine;
            while (*p && isspace(*p)) p++;

            if (*p == '[')
            {
                // about to leave the target section, insert the key at its end if still missing
                if (inTargetSection && !keyWritten)
                {
                    fprintf(out, "%s = %s\n", key, value);
                    keyWritten = true;
                }

                char* sectionEnd = strchr(p, ']');
                if (sectionEnd)
                {
                    *sectionEnd = '\0';
                    strncpy(currentSection, p + 1, sizeof(currentSection) - 1);
                    currentSection[sizeof(currentSection) - 1] = '\0';
                }
                inTargetSection = compareStrCaseInsensitive(currentSection, section);
                if (inTargetSection) sectionFound = true;

                fputs(line, out);
                continue;
            }

            if (*p == '#' || *p == ';' || *p == '\0')
            {
                fputs(line, out);
                continue;
            }

            if (inTargetSection && !keyWritten)
            {
                char* lineKey = NULL;
                char* lineValue = NULL;
                parseKeyValue(workLine, &lineKey, &lineValue);
                if (lineKey && compareStrCaseInsensitive(lineKey, key))
                {
                    fprintf(out, "%s = %s\n", key, value);
                    keyWritten = true;
                    continue;
                }
            }

            fputs(line, out);
        }
        fclose(in);
    }

    if (inTargetSection && !keyWritten)
    {
        fprintf(out, "%s = %s\n", key, value);
        keyWritten = true;
    }

    if (!sectionFound)
    {
        fprintf(out, "[%s]\n%s = %s\n", section, key, value);
    }

    fclose(out);

    remove(fileName);
    rename(tempFileName, fileName);
}

void writeKeyboardConfig(const char* fileName, const KeyboardConfig& keyboard)
{
    char buf[8];
    snprintf(buf, sizeof(buf), "%d", keyboard.keyUp);
    setIniValue(fileName, "Keyboard", "UP", buf);
    snprintf(buf, sizeof(buf), "%d", keyboard.keyDown);
    setIniValue(fileName, "Keyboard", "DOWN", buf);
    snprintf(buf, sizeof(buf), "%d", keyboard.keyLeft);
    setIniValue(fileName, "Keyboard", "LEFT", buf);
    snprintf(buf, sizeof(buf), "%d", keyboard.keyRight);
    setIniValue(fileName, "Keyboard", "RIGHT", buf);
    snprintf(buf, sizeof(buf), "%d", keyboard.keyJump);
    setIniValue(fileName, "Keyboard", "JUMP", buf);
    snprintf(buf, sizeof(buf), "%d", keyboard.keyAction);
    setIniValue(fileName, "Keyboard", "ACTION", buf);
}