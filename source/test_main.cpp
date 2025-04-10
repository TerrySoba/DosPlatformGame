#include "unit_test.h"
#include <string.h>
#include <stdio.h>


bool startsWith(const char* a, const char* b)
{
    int aLen = strlen(a);
    int bLen = strlen(b);

    if (aLen < bLen) return false;

    for (int i = 0; i < bLen; ++i)
    {
        if (a[i] != b[i]) return false;
    }

    return true;
}


int main(int argc, char* argv[])
{
    const char* filterString = "--filter=";
    int filterStringLen = strlen(filterString);

    char* filter = NULL;

    for (int i = 1; i < argc; ++i)
    {
        if (startsWith(argv[i], filterString)) {
            // printf("param %d: %s (%s)\n", i, argv[i], argv[i] + filterStringLen);
            filter = argv[i] + filterStringLen;
        }
    }
    
    bool success = runTests(filter);
    return success?0:1; 
}
