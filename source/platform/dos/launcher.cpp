// #include <stdlib.h>

#include <process.h>

// #include <stdio.h>

/*

void print(char *string);
#pragma aux print = \
        "mov ah, 09h"   \
        "int 0x21"      \
        parm    [dx]    \
        modify  [ax];

*/

void printCh(char ch);
#pragma aux printCh = \
        "mov ah, 02h"   \
        "int 0x21"      \
        parm    [dl]    \
        modify  [ax];

void printStr(const char* str)
{
    while (str && *str)
    {
        printCh(*str);
        ++str;
    }
}

// void printInteger(int value)
// {
//     char buffer[2];
//     buffer[1] = '$';
//     int pos = 0;
//     buffer[pos] = '0' + value;
//     print(buffer);
// }


int main(int argc, char* argv[])
{
    int ret = spawnv(P_WAIT, "title.exe", argv);
    if (ret == 1)
    {
        printStr("Launching game, please wait...\r\n");
        int ret = spawnv(P_WAIT, "game2.exe", argv);
    }
    return 0;
}