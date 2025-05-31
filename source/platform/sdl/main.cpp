#include "platform/sdl/sound_controller_sdl.h"
#include "platform/sdl/music_controller_sdl.h"
#include "platform/sdl/keyboard_sdl.h"
#include "framebuffer_gfx.h"
#include "shared_ptr.h"
#include "i18n.h"
#include "game.h"
#include "credits_scroller.h"
#include "exception.h"
#include "sdl_gfx.h"
#include "commandline_parser.h"
#include "sdl_context.h"

#include <memory>
#include <cmath>
#include <map>
#include <vector>

GameAnimations loadGameAnimations()
{
    tnd::shared_ptr<Animation> enemy(new Animation("enemy.ani", "enemy.tga"));
    tnd::shared_ptr<Animation> seekerEnemy(new Animation("enemy2.ani", "enemy2.tga"));
    tnd::shared_ptr<Animation> guffin(new Animation("guffin.ani", "guffin.tga"));
    tnd::shared_ptr<Animation> guy(new Animation("guy.ani", "guy.tga"));
    tnd::shared_ptr<Animation> fireBall(new Animation("fire.ani", "fire.tga"));
    tnd::shared_ptr<Animation> jetPack(new Animation("jet.ani", "jet.tga"));
    tnd::shared_ptr<Animation> tentacle(new Animation("tentacle.ani", "tentacle.tga"));
    tnd::shared_ptr<Animation> projectile(new Animation("bullet.ani", "bullet.tga"));
    tnd::shared_ptr<Animation> tentacleArm(new Animation("ten_arm.ani", "ten_arm.tga"));
    tnd::shared_ptr<Animation> eye(new Animation("eye.ani", "eye.tga"));

    GameAnimations animations = {guy, enemy, seekerEnemy, guffin, fireBall, jetPack, tentacle, projectile, tentacleArm, eye};
    return animations;
}

class GameWrapper {
public:
    GameWrapper(
        std::shared_ptr<SDL_Texture> renderTexture,
        std::shared_ptr<SDL_AudioStream> sfxAudioStream,
        std::shared_ptr<SDL_AudioStream> musicAudioStream) :
        m_renderTexture(renderTexture)
    {
        tnd::shared_ptr<SoundController> sound(new SoundControllerSdl(sfxAudioStream));

        m_gfx.reset(new FramebufferGfx());
        
        GameExitCode exitCode = GAME_EXIT_QUIT;
    
        tnd::shared_ptr<MusicController> music(new MusicControllerSdl(musicAudioStream));
        auto animations = loadGameAnimations();
        m_game.reset(new Game(m_gfx, sound, music, animations, "%02x%02x", LevelNumber(1,1)));
        
    }

    void drawFrame() {
        // Call the game render function here
        m_game->drawFrame();

        // lock the texture
        void* pixels;
        int pitch;
        SDL_Rect rect = {0,0,320,200};
        SDL_LockTexture(m_renderTexture.get(), NULL, &pixels, &pitch);

        m_gfx->renderToMemory(pixels, pitch, PixelFormat::PIXEL_FORMAT_RGBA8888);

        // unlock the texture
        SDL_UnlockTexture(m_renderTexture.get());

    }

    uint32_t getFrameCount() const {
        return m_game->getFrameCount();
    }

private:
    tnd::shared_ptr<SDL_Texture> m_renderTexture;
    tnd::shared_ptr<Game> m_game;
    tnd::shared_ptr<GfxOutput> m_gfx;
};

class ScreenSizeHelper {
public:
    ScreenSizeHelper(uint32_t windowWidth, uint32_t windowHeight, float aspectRatio)
    {
        setWindowSize(windowWidth, windowHeight, aspectRatio);
    }

    void setWindowSize(uint32_t width, uint32_t height, float aspectRatio)
    {
        m_windowWidth = width;
        m_windowHeight = height;
        m_aspectRatio = aspectRatio;

        if (m_aspectRatio > (float)width / (float)height) {
            // horizontal is the limiting factor
            m_renderWidth = m_windowWidth;
            m_renderHeight = static_cast<uint32_t>(m_windowWidth / aspectRatio);
        } else {
            // vertical is the limiting factor
            m_renderHeight = m_windowHeight;
            m_renderWidth = static_cast<uint32_t>(m_windowHeight * aspectRatio);
        }
      
        m_renderOffsetX = (m_windowWidth - m_renderWidth) / 2;
        m_renderOffsetY = (m_windowHeight - m_renderHeight) / 2;
    }

    uint32_t getRenderOffsetX() const
    {
        return m_renderOffsetX;
    }
    uint32_t getRenderOffsetY() const
    {
        return m_renderOffsetY;
    }
    uint32_t getRenderWidth() const
    {
        return m_renderWidth;
    }
    uint32_t getRenderHeight() const
    {
        return m_renderHeight;
    }

private:
    uint32_t m_windowWidth;
    uint32_t m_windowHeight;
    float m_aspectRatio;
    uint32_t m_renderWidth;
    uint32_t m_renderHeight;
    uint32_t m_renderOffsetX;
    uint32_t m_renderOffsetY;

};


int main(int argc, char* argv[]) {
    auto params = parseCommandLine(argc, argv);

    if (!params) {
        SDL_LogError(1, "Failed to parse command line arguments.");
        return 1;
    }

    auto renderDrivers = SdlContext::getAvailableRenderDrivers();
    
    if (params->sdlRenderer == "list") {
        SDL_Log("Available renderer drivers:");
        int i = 1;
        for (auto renderer : renderDrivers)
        {
            SDL_Log("%d. %s", i++, renderer.c_str());
        }
        return 1;
    }

    const uint32_t gameWindowResolutionWidth = params->screenWidth;
    const uint32_t gameWindowResolutionHeight = params->screenHeight;
    const float dosGameAspectRatio = 4.0 / 3.0;

    try {

        if (params->language == "english") {
            I18N::loadTranslations("strings.en");
        }
        else if (params->language == "german") {
            I18N::loadTranslations("strings.de");
        }
        else
        {
            THROW_EXCEPTION("Unknown language: ", params->language.c_str());
        }

        SdlContext sdl(
            params->fullscreen,
            gameWindowResolutionWidth,
            gameWindowResolutionHeight,
            params->sdlRenderer);

        bool quit = false;
        SDL_Event e;


        GameWrapper gameWrapper(sdl.getTexture(), sdl.getSfxStream(), sdl.getBgmStream());

        uint32_t targetFps = 70;

        uint64_t targetFrameTimeNs = 1e9 / targetFps;
        

        ScreenSizeHelper screenSizeHelper(gameWindowResolutionWidth, gameWindowResolutionHeight, dosGameAspectRatio);

        int64_t frames = 0;
        int64_t sleepAdjustment = 0;

        KeyboardSdl keyboard;

        // run main game loop
        uint64_t lastFrameTimeNs = SDL_GetTicksNS();
        uint64_t frameCounterStartTime = lastFrameTimeNs;
        std::vector<int16_t> musicBuffer(960 * 2);

        bool displayMenu = false;
        while (!quit)
        {
            int64_t now = SDL_GetTicksNS();
            int64_t nsSinceLastFrame = now - lastFrameTimeNs;
            lastFrameTimeNs = now;
            if (nsSinceLastFrame > targetFrameTimeNs) {
                sleepAdjustment -= 1e4;
            } else {
                sleepAdjustment += 1e4;
            }

            ++frames;
            
            if (!displayMenu)
            {
                gameWrapper.drawFrame();
            }
            
            SDL_RenderClear(sdl.getRenderer().get());
            SDL_FRect dst = {
                (float)screenSizeHelper.getRenderOffsetX(), (float)screenSizeHelper.getRenderOffsetY(),
                (float)screenSizeHelper.getRenderWidth(), (float)screenSizeHelper.getRenderHeight()};
            SDL_RenderTexture(sdl.getRenderer().get(), sdl.getTexture().get(), nullptr, &dst);

            SDL_RenderPresent(sdl.getRenderer().get());

            // handle events
            while (SDL_PollEvent(&e)) {
                switch (e.type) {
                    case SDL_EVENT_QUIT:
                        quit = true;
                        break;
                    case SDL_EVENT_KEY_DOWN:
                    {
                        keyboard.keyDown(e.key.key);
                        break;
                    }
                    case SDL_EVENT_KEY_UP:
                    {
                        keyboard.keyUp(e.key.key);
                        break;
                    }
                    default:
                        break;
                }
            }

            if (s_keyEsc)
            {
                quit = true;
            }
 
            // wait for the next frame
            int64_t sleepTimeNs = targetFrameTimeNs - (SDL_GetTicksNS() - lastFrameTimeNs);
            if (sleepTimeNs > 0) {
                SDL_DelayPrecise(sleepTimeNs + sleepAdjustment);
            }
        }

        // Print the frame count
        uint64_t frameCounterEndTime = SDL_GetTicksNS();
        uint64_t frameCounterDuration = frameCounterEndTime - frameCounterStartTime;
        double fps = (double)frames / ((double)frameCounterDuration / 1e9);
        SDL_Log("FPS: %f", fps);
        SDL_Log("Frames: %ld", frames);



        SDL_Log("Quitting SDL.");
        

        SDL_Log("Goodbye, frames: %d", gameWrapper.getFrameCount());
    }
    catch(const Exception& e)
    {
        SDL_LogError(1, "Exception: %s\r\nerrno: %d", e.what(), errno);
        return 1;
    }
    catch(const std::exception& e)
    {
        SDL_LogError(1, "std::exception: %s\r\nerrno: %d", e.what(), errno);
        return 1;
    }
    catch(...)
    {
        SDL_LogError(1, "Unknown exception.\r\n");
        return 1;
    }


    return 0;
}
