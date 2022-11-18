// #include <stdlib.h>

#include <process.h>

// #include <stdio.h>

void print(char *string);
#pragma aux print = \
        "mov ah, 09h"   \
        "int 0x21"      \
        parm    [dx]    \
        modify  [ax];


void printInteger(int value)
{
    char buffer[2];
    buffer[1] = '$';
    int pos = 0;
    buffer[pos] = '0' + value;
    print(buffer);
}

int main(int argc, char* argv[])
{
    int ret = spawnl(P_WAIT, "title.exe", 0);
    if (ret == 1)
    {
        print("Launching game, please wait...\n$");
        system("game2.exe");
    }
    return 0;
}