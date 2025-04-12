#include "platform/dos/vgagfx.h"
#include "tga_image.h"
#include "platform/dos/keyboard.h"
#include "font.h"
#include "font_writer.h"
#include "animation.h"
#include "exception.h"
#include "i18n.h"

#include <stdio.h>
#include <stdlib.h>
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

enum ExitCode
{
    EXIT_CODE_NONE = 0,
    EXIT_CODE_START_GAME = 1,
    EXIT_CODE_QUIT = 2
};


#define START_GAME_POS_X 162
#define START_GAME_POS_Y 88

#define DELETE_SAVE_POS_X 162
#define DELETE_SAVE_POS_Y 108

#define EXIT_POS_X 162
#define EXIT_POS_Y 128


void deleteSavegame()
{
    remove("GAME.SAV");
}


int main(int argc, char* argv[])
{
    bool useGerman = false;

    for (int i = 1; i < argc; ++i)
    {
        if (strcmp("--german", argv[i]) == 0)	
        {
            useGerman = true;
        }
    }

    try
    {
        if (useGerman) I18N::loadTranslations("strings.de");
        else I18N::loadTranslations("strings.en");

        Keyboard keys;
        VgaGfx vga;
        Animation arrow("arrow2.ani", "arrow2.tga", true);
        TgaImage image("pyramid.tga");
        Font font("lib18.stf");
        FontWriter fontWriter(&font);
        

        vga.setBackground(image);

        fontWriter.setText(I18N::getString(42).c_str());
        vga.drawBackground(fontWriter, START_GAME_POS_X + 17, START_GAME_POS_Y - 2);

        fontWriter.setText(I18N::getString(43).c_str());
        vga.drawBackground(fontWriter, DELETE_SAVE_POS_X + 17, DELETE_SAVE_POS_Y - 2);

        fontWriter.setText(I18N::getString(44).c_str());
        vga.drawBackground(fontWriter, EXIT_POS_X + 17, EXIT_POS_Y - 2);

        TitleState state = STATE_INIT;

        int arrowX = 0;
        int arrowY = 0;

        bool up = false;
        bool lastUp = false;
        bool down = false;
        bool lastDown = false;

        float arrowSpeed = 1;

        uint8_t counter = 0;

        ExitCode exitCode = EXIT_CODE_NONE;

        while (exitCode == EXIT_CODE_NONE)
        {
            up = s_keyUp;
            down = s_keyDown;

            vga.clear();
            vga.draw(arrow, arrowX, arrowY);
            vga.drawScreen();


            if (counter++ & 4)
            {
                arrow.nextFrame();
            }

            int actionButton = s_keySpace || s_keyAlt;
            int exitButton = s_keyEsc;

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
                    if (actionButton) state = STATE_START_GAME_SELECTED;
                    break;
                case STATE_START_GAME_SELECTED:
                    arrowX += arrowSpeed;
                    arrowSpeed *= 1.1;
                    if (arrowX > 320)
                    {
                        // return 0;
                        exitCode = EXIT_CODE_START_GAME;
                    }
                    break;
                case STATE_DELETE_SAVEGAME:
                    arrowSpeed = 1;
                    arrowX = DELETE_SAVE_POS_X;
                    arrowY = DELETE_SAVE_POS_Y;
                    if (up && !lastUp) state = STATE_START_GAME;
                    if (down && !lastDown) state = STATE_EXIT;
                    if (actionButton) {
                        deleteSavegame();
                        state = STATE_DELETE_SAVEGAME_SELECTED;
                    }
                    break;
                case STATE_DELETE_SAVEGAME_SELECTED:
                    arrowX += arrowSpeed;
                    arrowSpeed *= 1.1;
                    if (arrowX > 320)
                    {
                        state = STATE_DELETE_SAVEGAME;
                    }
                    break;
                case STATE_EXIT:
                    arrowSpeed = 1;
                    arrowX = EXIT_POS_X;
                    arrowY = EXIT_POS_Y;
                    if (up && !lastUp) state = STATE_DELETE_SAVEGAME;
                    if (down && !lastDown) state = STATE_START_GAME;
                    if (actionButton) state = STATE_EXIT_SELECTED;
                    break;
                case STATE_EXIT_SELECTED:
                    arrowX += arrowSpeed;
                    arrowSpeed *= 1.1;
                    if (arrowX > 320)
                    {
                        return 0;
                    }
                    break;
                default:
                    state = STATE_INIT;
                    break;
            }

            lastDown = down;
            lastUp = up;

            if (exitButton) exitCode = EXIT_CODE_QUIT;
        }

        return exitCode;
    }
    catch(const Exception& e)
    {
        fprintf(stderr, "Exception: %s\n", e.what());
        return 1;
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