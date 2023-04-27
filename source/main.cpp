#include "vgagfx.h"
#include "image.h"
#include "animation.h"
#include "keyboard.h"
#include "rad_player.h"
#include "tga_image.h"
#include "level.h"
#include "game.h"
#include "version.h"
#include "shared_ptr.h"
#include "joystick.h"
#include "text.h"
#include "i18n.h"
#include "sound_controller.h"
#include <dos.h>

#include "exception.h"

#include <stdio.h>


enum Language {
    LANGUAGE_ENGLISH,
    LANGUAGE_GERMAN,
};

struct CommandLineParametes {
    LevelNumber level;
    Language language;
    bool dumpLevelImages; // if set to true the game will dump each map as a graphics image
};


CommandLineParametes parseCommandline(int argc, char* argv[])
{
    CommandLineParametes params;
    params.language = LANGUAGE_ENGLISH;
    LevelNumber level = {1, 1};
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
    try
    {
        shared_ptr<SoundController> sound(new SoundController);

        CommandLineParametes params = parseCommandline(argc, argv);
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

        
        Keyboard keyboard;

        shared_ptr<ImageBase> tiles(new TgaImage("tiles.tga"));
        
        shared_ptr<Animation> enemy(new Animation("enemy.ani", "enemy.tga"));
        shared_ptr<Animation> seekerEnemy(new Animation("enemy2.ani", "enemy2.tga"));
        shared_ptr<Animation> guffin(new Animation("guffin.ani", "guffin.tga"));
        shared_ptr<Animation> guy(new Animation("guy.ani", "guy.tga"));
        shared_ptr<Animation> fireBall(new Animation("fire.ani", "fire.tga"));
        shared_ptr<Animation> jetPack(new Animation("jet.ani", "jet.tga"));

        shared_ptr<VgaGfx> gfx(new VgaGfx);
        
        RadPlayer music("CELT.RAD");

        GameAnimations animations = {guy, enemy, seekerEnemy, guffin, fireBall, jetPack};

        Game game(gfx, sound, tiles, animations, "%02x%02x", params.level);

        if (params.dumpLevelImages)
        {
            for (int x = 0; x < 16; ++x)
            {
                for (int y = 0; y < 16; ++y)
                {
                    LevelNumber level = {x,y};
                    try {
                        game.loadLevel(level, ActorPosition::LevelTransition);
                        game.drawFrame();
                        char filename[16];
                        sprintf(filename, "%02x%02x.tga", x, y);
                        gfx->saveAsTgaImage(filename);
                    } catch (...)
                    {}
                }
            }
        }
        else
        {
            while (!s_keyEsc)
            {	
                game.drawFrame();
            }
        }
        
    }
    catch(const Exception& e)
    {
        fprintf(stderr, "Exception: %s\n", e.what());
        fprintf(stderr, "errno: %d\n", errno);
        return 1;
    }
    catch(...)
    {
        fprintf(stderr, "Unknown exception.");
        return 1;
    }

    printf(I18N::getString(3).c_str(), BUILD_DATE);

    return 0;
}
