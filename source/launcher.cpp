// #include <stdlib.h>

#include <process.h>

// #include <stdio.h>

void print(char *string);
#pragma aux print = \
        "mov ah, 09h"   \
        "int 0x21"      \
        parm    [dx]    \
        modify  [ax];

// void printCh(char ch);
// #pragma aux printCh = \
//         "mov ah, 02h"   \
//         "int 0x21"      \
//         parm    [dl]    \
//         modify  [ax];

// void printStr(const char* str)
// {
//     while (str && *str)
//     {
//         printCh(*str);
//         ++str;
//     }
// }

// void printInteger(int value)
// {
//     char buffer[2];
//     buffer[1] = '$';
//     int pos = 0;
//     buffer[pos] = '0' + value;
//     print(buffer);
// }


bool stringsEqual(const char* str1, const char* str2)
{
    if (str1 == 0 || str2 == 0)
    {
        return false;
    }

    while (*str1 && *str2)
    {
        if (*str1 != *str2){
            return false;
        } 
    
        ++str1; ++str2;
    }
    return (*str1 == 0 && *str2 == 0);
}

int main(int argc, char* argv[])
{
    bool german = false;
    
    for (int i = 1; i < argc; ++i)
    {
        if (stringsEqual(argv[i], "--german"))
        {
            german = true;
        }
    }

    int ret = spawnl(P_WAIT, "title.exe", 0);
    if (ret == 1)
    {
        print("Launching game, please wait...\r\n$");
        if (german)
        {
            system("game2.exe --german");
        }
        else
        {
            system("game2.exe");
        }
    }
    return 0;
}