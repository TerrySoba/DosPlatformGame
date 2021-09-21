#ifndef _LOG_INCLUDED
#define _LOG_INCLUDED

#include <stdio.h>

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#define LOG_ENTRY(message, ...) \
    { \
        FILE* fp = fopen("logfile.txt", "ab"); \
        fprintf(fp, message " ("  __FILE__ ":" STR(__LINE__) ")\n", ##__VA_ARGS__); \
        fclose(fp); \
    }


#endif