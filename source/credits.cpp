#include "credits_scroller.h"
#include "vgagfx.h"
#include "exception.h"

#include <stdio.h>

int main(int argc, char* argv[])
{
    try
    {
        VgaGfx vga;
        runCredits(vga, "credits.txt");
    }
    catch(const Exception& e)
    {
        fprintf(stderr, "Exception: %s\n", e.what());
        return 1;
    }
    catch(...)
    {
        fprintf(stderr, "Unknown exception.");
        return 1;
    }
    
    return 0;
}
