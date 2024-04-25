#include <stdio.h>

#include "vgagfx.h"
#include "keyboard.h"
#include "tga_image.h"

int main(int argc, char *argv[]) {
    
    Keyboard keyboard;
    VgaGfx vgaGfx;
    TgaImage title("sungrav.tga");
    TgaImage tiles("tiles.tga");

    vgaGfx.setBackground(tiles);

    vgaGfx.clear();
    vgaGfx.drawScreen();
    vgaGfx.vsync();



    vgaGfx.fancyWipe(title);
    while (!s_keyEsc)
    {
        
    }

    return 0;
}
