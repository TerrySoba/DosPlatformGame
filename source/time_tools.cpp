#include "time_tools.h"

#include <stdint.h>
#include <stdio.h>

static char timeString[32];

char* createTimeString(uint32_t frames)
{
    uint32_t seconds = frames / 70;
    uint32_t minutes = seconds / 60;
    uint32_t hours = minutes / 60;
    minutes = minutes % 60;
    seconds = seconds % 60;
    frames = frames % 70;
    sprintf(timeString, "%02ld:%02ld:%02ld.%03ld\n", hours, minutes, seconds, frames * 1000 / 70);
    return timeString;
}

// function that converts frames into a time string
void printTime(uint32_t frames)
{
    printf("%s", createTimeString(frames));
}
