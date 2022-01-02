#include "vgagfx.h"
#include "tga_image.h"
#include "keyboard.h"

#include "animation.h"

#include <stdio.h>
#include <exception>
#include <string.h>


enum TitleState
{
    STATE_INIT,
    STATE_START_GAME,
    STATE_START_GAME_SELECTED,
    STATE_DELETE_SAVEGAME,
    STATE_DELETE_SAVEGAME_SELECTED,
    STATE_EXIT,
    STATE_EXIT_SELECTED,
};


#define START_GAME_POS_X 150
#define START_GAME_POS_Y 124

#define DELETE_SAVE_POS_X 150
#define DELETE_SAVE_POS_Y 144

#define EXIT_POS_X 150
#define EXIT_POS_Y 164


void deleteSavegame()
{
    remove("GAME.SAV");
}


int main(int argc, char* argv[])
{
    try
    {
        Keyboard keys;
        VgaGfx vga;
        Animation arrow("arrow2.ani", "arrow2.tga", true);
        TgaImage image("title.tga");

        vga.setBackground(image);

        TitleState state = STATE_INIT;

        int arrowX = 0;
        int arrowY = 0;

        bool up = false;
        bool lastUp = false;
        bool down = false;
        bool lastDown = false;

        float arrowSpeed = 1;

        uint8_t counter = 0;

        while (true)
        {
            up = s_keyUp;
            down = s_keyDown;

            vga.clear();
            vga.draw(arrow, arrowX, arrowY);
            vga.drawScreen();


            if (counter++ & 4)
                arrow.nextFrame();

            switch(state)
            {
                case STATE_INIT:
                    state = STATE_START_GAME;
                    break;
                case STATE_START_GAME:
                    arrowSpeed = 1;
                    arrowX = START_GAME_POS_X;
                    arrowY = START_GAME_POS_Y;
                    if (up && !lastUp) state = STATE_EXIT;
                    if (down && !lastDown) state = STATE_DELETE_SAVEGAME;
                    if (s_keySpace) state = STATE_START_GAME_SELECTED;
                    break;
                case STATE_START_GAME_SELECTED:
                    arrowX += arrowSpeed;
                    arrowSpeed *= 1.1;
                    if (arrowX > 320) return 0;
                    break;
                case STATE_DELETE_SAVEGAME:
                    arrowSpeed = 1;
                    arrowX = DELETE_SAVE_POS_X;
                    arrowY = DELETE_SAVE_POS_Y;
                    if (up && !lastUp) state = STATE_START_GAME;
                    if (down && !lastDown) state = STATE_EXIT;
                    if (s_keySpace) {
                        deleteSavegame();
                        state = STATE_DELETE_SAVEGAME_SELECTED;
                    }
                    break;
                case STATE_DELETE_SAVEGAME_SELECTED:
                    arrowX += arrowSpeed;
                    arrowSpeed *= 1.1;
                    if (arrowX > 320) state = STATE_DELETE_SAVEGAME;
                    break;
                case STATE_EXIT:
                    arrowSpeed = 1;
                    arrowX = EXIT_POS_X;
                    arrowY = EXIT_POS_Y;
                    if (up && !lastUp) state = STATE_DELETE_SAVEGAME;
                    if (down && !lastDown) state = STATE_START_GAME;
                    if (s_keySpace) state = STATE_EXIT_SELECTED;
                    break;
                case STATE_EXIT_SELECTED:
                    arrowX += arrowSpeed;
                    arrowSpeed *= 1.1;
                    if (arrowX > 320) return 1;
                    break;
                default:
                    state = STATE_INIT;
                    break;
            }

            lastDown = down;
            lastUp = up;
        }
    }
    catch(const std::exception& e)
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