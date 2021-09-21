#include "unit_test.h"
#include "tiny_string.h"
#include "vector.h"
#include <string.h>


tnd::vector<int> getSpacePositions(const char* text)
{
    tnd::vector<int> spacePositions;
    const char* ptr = text;
    int pos = 0;
    while (*ptr != '\0')
    {
        if (*ptr == ' ') spacePositions.push_back(pos);
        ++pos;
        ++ptr;
    }

    spacePositions.push_back(pos);
    return spacePositions;
}


void adjustSpaces(char* data, int length)
{
    // find number of spaces at end of line
    int spaces = 0;
    for (int i = length - 1; i > 0; --i)
    {
        if (data[i] != ' ') break;
        ++spaces;
    }

    // insert spaces to make line full length
    while (spaces > 0)
    {
        char lastCh = 0;
        for (int i = 0; i < length - spaces; ++i)
        {
            if (lastCh != ' ' && data[i] == ' ' && spaces > 0)
            {
                memmove(&data[i+1], &data[i], length-i-1);
                --spaces;
                ++i;
            }
            lastCh = data[i];
        }
    }
}



tnd::vector<TinyString> formatString2(const char* str, int lineWidth)
{
    tnd::vector<int> spaces = getSpacePositions(str);

    tnd::vector<TinyString> out;

    int diff = 0;
    for (int i = 0; i < spaces.size(); ++i)
    {
        if (spaces[i] - diff >= lineWidth)
        {
            TinyString line(lineWidth);
            memcpy(line.data(), &str[diff], spaces[i-1] - diff);
            adjustSpaces(line.data(), lineWidth);
            out.push_back(line);
            diff = spaces[i-1]+1;
        }
    }

    TinyString line(lineWidth);
    memcpy(line.data(), &str[diff], spaces[spaces.size()-1] - diff);
    adjustSpaces(line.data(), lineWidth);
    out.push_back(line);

    return out;
}


TEST(TextLayoutTest1)
{
    const char* testText = "test text is a test text";

    tnd::vector<int> spaces = getSpacePositions(testText);
    ASSERT_TRUE(spaces.size() == 6);
    ASSERT_TRUE(spaces[spaces.size()-1] == strlen(testText));

    ASSERT_TRUE(spaces[0] == 4);
    ASSERT_TRUE(spaces[1] == 9);
    ASSERT_TRUE(spaces[2] == 12);
    ASSERT_TRUE(spaces[3] == 14);
    ASSERT_TRUE(spaces[4] == 19);
    ASSERT_TRUE(spaces[5] == 24);
}

TEST(TextLayoutTest2)
{
    const char* testText = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Curabitur gravida bibendum enim sodales ornare.";
    tnd::vector<TinyString> resultText;
    resultText.push_back(TinyString("Lorem   ipsum  dolor  sit  amet,"));
    resultText.push_back(TinyString("consectetur   adipiscing   elit."));
    resultText.push_back(TinyString("Curabitur  gravida bibendum enim"));
    resultText.push_back(TinyString("sodales                  ornare."));

    tnd::vector<TinyString> f = formatString2(testText, 32);

    ASSERT_TRUE(resultText.size() == f.size());

    for (int i = 0; i < resultText.size(); ++i)
    {
        ASSERT_TRUE(resultText[i] == f[i]);
    }
}

TEST(AdjustSpaces)
{
    TinyString testString("Test string 123     ");
    adjustSpaces(testString.data(), testString.size());
    ASSERT_TRUE(testString == TinyString("Test    string   123"));
}