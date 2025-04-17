#include "platform/dos/vgagfx.h"
#include "platform/dos/keyboard_dos.h"
#include "platform/dos/joystick_dos.h"
#include "platform/dos/sound_controller_dos.h"
#include "platform/dos/music_controller_dos.h"

#include "image.h"
#include "animation.h"

#include "tga_image.h"
#include "level.h"
#include "game.h"
#include "version.h"
#include "text.h"
#include "i18n.h"
#include "sound_controller.h"
#include "shared_ptr.h"
#include "credits_scroller.h"
#include <dos.h>

#include "exception.h"

enum Language {
    LANGUAGE_ENGLISH,
    LANGUAGE_GERMAN,
};

struct CommandLineParameters {
    LevelNumber level;
    Language language;
    bool dumpLevelImages; // if set to true the game will dump each map as a graphics image
};


CommandLineParameters parseCommandline(int argc, char* argv[])
{
    CommandLineParameters params;
    params.language = LANGUAGE_ENGLISH;
    LevelNumber level(1, 1);
    params.dumpLevelImages = false;

    for (int i = 1; i < argc; ++i)
    {
        if (strcmp("--level", argv[i]) == 0 && i+2 < argc)
        {
            int16_t levelX, levelY;
            sscanf(argv[i+1], "%d", &levelX);
            sscanf(argv[i+2], "%d", &levelY);
            i+=2;

            level.x = levelX;
            level.y = levelY;
        }
        if (strcmp("--english", argv[i]) == 0)	
        {
            params.language = LANGUAGE_ENGLISH;
        }
        if (strcmp("--german", argv[i]) == 0)
        {
            params.language = LANGUAGE_GERMAN;
        }
        if (strcmp("--dump-level-images", argv[i]) == 0)
        {
            params.dumpLevelImages = true;
        }
    }

    params.level = level;
    return params;
}


int main(int argc, char* argv[])
{
    uint32_t frameCounter = 0;

    try
    {
        tnd::shared_ptr<SoundController> sound = new SoundControllerDos();

        CommandLineParameters params = parseCommandline(argc, argv);
        calibrateJoystick();
        
        switch(params.language)
        {
            case LANGUAGE_GERMAN:
                I18N::loadTranslations("strings.de");
                break;
            case LANGUAGE_ENGLISH:
            default:
                I18N::loadTranslations("strings.en");
        }

        tnd::shared_ptr<GfxOutput> gfx(new VgaGfx());
        Keyboard keyboard;
    
        GameExitCode exitCode = GAME_EXIT_QUIT;

        {
            tnd::shared_ptr<Animation> enemy = new Animation("enemy.ani", "enemy.tga");
            tnd::shared_ptr<Animation> seekerEnemy = new Animation("enemy2.ani", "enemy2.tga");
            tnd::shared_ptr<Animation> guffin = new Animation("guffin.ani", "guffin.tga");
            tnd::shared_ptr<Animation> guy = new Animation("guy.ani", "guy.tga");
            tnd::shared_ptr<Animation> fireBall = new Animation("fire.ani", "fire.tga");
            tnd::shared_ptr<Animation> jetPack = new Animation("jet.ani", "jet.tga");
            tnd::shared_ptr<Animation> tentacle = new Animation("tentacle.ani", "tentacle.tga");
            tnd::shared_ptr<Animation> projectile = new Animation("bullet.ani", "bullet.tga");
            tnd::shared_ptr<Animation> tentacleArm = new Animation("ten_arm.ani", "ten_arm.tga");
            tnd::shared_ptr<Animation> eye = new Animation("eye.ani", "eye.tga");

            tnd::shared_ptr<MusicController> music(new MusicControllerDos());

            GameAnimations animations = {guy, enemy, seekerEnemy, guffin, fireBall, jetPack, tentacle, projectile, tentacleArm, eye};

            Game game(gfx, sound, music, animations, "%02x%02x", params.level);

            if (params.dumpLevelImages)
            {
                for (int x = 0; x < 32; ++x)
                {
                    for (int y = 0; y < 16; ++y)
                    {
                        LevelNumber level(x,y);
                        try {
                            game.loadLevel(level, ActorPosition::LevelTransition);
                            game.drawFrame();
                            char xBuf[3];
                            char yBuf[3];
                            intToString(x, 16, xBuf, 3, 2, '0');
                            intToString(y, 16, yBuf, 3, 2, '0');
                            TinyString filename = TinyString(xBuf) + TinyString(yBuf) + TinyString(".tmx");
                            Text t(filename.c_str(), 10, false);
                            gfx->draw(t, 100, 100);
                            gfx->drawScreen();
                            filename = TinyString(xBuf) + TinyString(yBuf) + TinyString(".tga");
                            gfx->saveAsTgaImage(filename.c_str());
                        } catch (...)
                        {}
                    }
                }
            }
            else
            {
                exitCode = game.runGameLoop();
            }
            frameCounter = game.getFrameCount();
        }

        if (exitCode == GAME_EXIT_CREDITS)
        {
            runCredits(*gfx, "credits.txt");
        }
        
    }
    catch(const Exception& e)
    {
        printStr("Exception: ");
        printStr(e.what());
        printStr("\r\nerrno: ");

        char buf[10];
        intToString(errno, 10, buf, 10);
        printStr(buf);
        printStr("\r\n");
        return 1;
    }
    catch(...)
    {
        printStr("Unknown exception.\r\n");
        return 1;
    }

    printStr(BUILD_DATE + TinyString("\r\n") + I18N::getString(3).c_str());

    return 0;
}
