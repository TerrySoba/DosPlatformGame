#include "time_tools.h"
#include "tiny_string.h"

#include <stdint.h>

static const uint16_t TIME_STRING_SIZE = 32;

static char timeString[TIME_STRING_SIZE];

char* createTimeString(uint32_t frames)
{
    uint32_t seconds = frames / 70;
    uint32_t minutes = seconds / 60;
    uint32_t hours = minutes / 60;
    minutes = minutes % 60;
    seconds = seconds % 60;
    frames = frames % 70;

    uint16_t strPos = 0;
    strPos += intToString(hours, 10, timeString, TIME_STRING_SIZE - strPos, 2, '0');
    timeString[strPos++] = ':';
    strPos += intToString(minutes, 10, timeString+strPos, TIME_STRING_SIZE - strPos, 2, '0');
    timeString[strPos++] = ':';
    strPos += intToString(seconds, 10, timeString+strPos, TIME_STRING_SIZE - strPos, 2, '0');
    timeString[strPos++] = '.';
    intToString(frames * 1000 / 70, 10, timeString+strPos, TIME_STRING_SIZE - strPos, 3, '0');

    return timeString;
}

// function that converts frames into a time string
void printTime(uint32_t frames)
{
    printStr(createTimeString(frames));
}
