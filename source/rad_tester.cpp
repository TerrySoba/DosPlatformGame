#include "rad_player.h"
#include "keyboard.h"
#include "exception.h"
#include <stdio.h>

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        printf("Usage: %s <rad module>\n", argv[0]);

        return 1;
    }
    try
    {
        Keyboard keyboard;
        RadPlayer player;
        player.playModule(argv[1]);
        printf("Playing module %s\n", argv[1]);
        printf("Press ESC to stop.\n");
        while (!s_keyEsc);
        printf("Done.\n");
    }
    catch(const Exception& e)
    {
        printf("Exception: %s\n", e.what());
    }
    return 0;
}
