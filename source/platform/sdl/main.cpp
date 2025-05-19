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
#include "decode_vorbis.h"

#include "imgui.h"
#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_sdlrenderer3.h"

#include <SDL3/SDL.h>

#include <memory>
#include <cmath>
#include <map>
#include <vector>

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

            tnd::shared_ptr<MusicController> music(new MusicControllerSdl(musicAudioStream));

            GameAnimations animations = {guy, enemy, seekerEnemy, guffin, fireBall, jetPack, tentacle, projectile, tentacleArm, eye};

            m_game.reset(new Game(m_gfx, sound, music, animations, "%02x%02x", LevelNumber(1,1)));
        }
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

void setupImGui(SDL_Window* window, SDL_Renderer* renderer)
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);
}

void startImGuiFrame()
{
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
}

void renderImGui(SDL_Renderer* renderer)
{
    ImGui::Render();
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
}


std::vector<std::string> getAvailableRenderDrivers()
{
    std::vector<std::string> drivers;
    int numDrivers = SDL_GetNumRenderDrivers();
    for (int i = 0; i < numDrivers; i++)
    {
        const char* name = SDL_GetRenderDriver(i);
        if (name)
        {
            drivers.push_back(name);
        }
    }
    return drivers;
}

void displayGameSettingsWindow(
    std::vector<std::string> &renderDrivers,
    std::string &selectedRenderer,
    bool& quitGame,
    bool& displayMenu)
{
    static bool music = true;
    static bool effects = true;
    static bool fullscreen = false;
    static bool settingsOpen = false;

    ImGui::Begin("Game Menu", &displayMenu);

    if (ImGui::Button("Settings"))
    {
        settingsOpen = true;
    }

    if (ImGui::Button("Continue"))
    {
        displayMenu = false;
    }

    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0, 0.6f, 0.6f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0, 0.7f, 0.7f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0, 0.8f, 0.8f));
    if (ImGui::Button("Quit Game"))
    {
        quitGame = true;
    }
    ImGui::PopStyleColor(3);


    ImGui::End();

    if (settingsOpen)
    {
        ImGui::Begin("Game Settings", &settingsOpen);
        if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None))
        {
            if (ImGui::BeginTabItem("Sound"))
            {
                ImGui::Checkbox("Music", &music);
                ImGui::Checkbox("Effects", &effects);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Graphics"))
            {
                ImGui::Checkbox("Fullscreen", &fullscreen);

                if (ImGui::BeginCombo("Renderer", selectedRenderer.c_str()))
                {
                    for (const auto &driver : renderDrivers)
                    {
                        bool isSelected = (driver == selectedRenderer);
                        if (ImGui::Selectable(driver.c_str(), isSelected))
                        {
                            selectedRenderer = driver;
                        }
                        if (isSelected)
                        {
                            ImGui::SetItemDefaultFocus();
                        }
                    }

                    ImGui::EndCombo();
                }

                ImGui::Button("Apply");
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
        ImGui::End();
    }

    
}

int main(int argc, char* argv[]) {
    auto params = parseCommandLine(argc, argv);

    if (!params) {
        SDL_LogError(1, "Failed to parse command line arguments.");
        return 1;
    }

    auto renderDrivers = getAvailableRenderDrivers();
    

    if (params->sdlRenderer == "list") {
        SDL_Log("Available renderer drivers:");
        // for (int i = 0; i < SDL_GetNumRenderDrivers(); i++)
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

        SDL_Log("Initializing SDL.");

        if (!SDL_InitSubSystem(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
            THROW_EXCEPTION("SDL_InitSubSystem Error: ", SDL_GetError());
        }

        std::shared_ptr<void> sdlCleanup(nullptr, [](void*) { SDL_Quit(); });

        SDL_AudioSpec spec;
        spec.freq = 48000;
        spec.format = SDL_AUDIO_S16;
        spec.channels = 2;

        SDL_AudioDeviceID deviceId = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec);
        if (deviceId == 0) {
            SDL_LogError(1, "SDL_OpenAudioDevice Error: %s", SDL_GetError());
            THROW_EXCEPTION("SDL_OpenAudioDevice Error: ", SDL_GetError());
        }
        SDL_ResumeAudioDevice(deviceId);

        auto bgmStream = std::shared_ptr<SDL_AudioStream>(SDL_CreateAudioStream(&spec, &spec), SDL_DestroyAudioStream);
        if (!bgmStream) {
            SDL_LogError(1, "SDL_CreateAudioStream Error: %s", SDL_GetError());
            THROW_EXCEPTION("SDL_CreateAudioStream Error: ", SDL_GetError());
        }

        if (!SDL_BindAudioStream(deviceId, bgmStream.get()))
        {
            SDL_LogError(1, "SDL_BindAudioStream Error: %s", SDL_GetError());
            THROW_EXCEPTION("SDL_BindAudioStream Error: ", SDL_GetError());
        }

        auto sfxStream = std::shared_ptr<SDL_AudioStream>(SDL_CreateAudioStream(&spec, &spec), SDL_DestroyAudioStream);
        if (!sfxStream) {
            SDL_LogError(1, "SDL_CreateAudioStream Error: %s", SDL_GetError());
            THROW_EXCEPTION("SDL_CreateAudioStream Error: ", SDL_GetError());
        }

        if (!SDL_BindAudioStream(deviceId, sfxStream.get()))
        {
            SDL_LogError(1, "SDL_BindAudioStream Error: %s", SDL_GetError());
            THROW_EXCEPTION("SDL_BindAudioStream Error: ", SDL_GetError());
        }


        // VorbisDecoder decoder("/home/yoshi252/Documents/lmms/projects/opl2_explort_filters.ogg");

        const char *videoDriver = SDL_GetCurrentVideoDriver();
        if (videoDriver)
        {
            SDL_Log("Current video driver: %s", videoDriver);
        }
        else
        {
            SDL_Log("Failed to get video driver.");
        }

        SDL_WindowFlags windowFlags = 0;
        if (params->fullscreen) {
            windowFlags |= SDL_WINDOW_FULLSCREEN;
            SDL_HideCursor();
        }

        std::shared_ptr<SDL_Window> win(SDL_CreateWindow("SdlPlatformGame", gameWindowResolutionWidth, gameWindowResolutionHeight, windowFlags), SDL_DestroyWindow);
        if (!win) {
            THROW_EXCEPTION("SDL_CreateWindow Error: ", SDL_GetError());
        }

        const char* rendererName = params->sdlRenderer.empty() ? nullptr : params->sdlRenderer.c_str();

        std::shared_ptr<SDL_Renderer> ren(SDL_CreateRenderer(win.get(), rendererName), SDL_DestroyRenderer);
        if (!ren) {
            THROW_EXCEPTION("SDL_CreateRenderer Error: ", SDL_GetError());
        }

        // get name of renderer
        std::string selectedRenderer = SDL_GetRendererName(ren.get());

        std::shared_ptr<SDL_Texture> tex(SDL_CreateTexture(ren.get(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 320, 200), SDL_DestroyTexture);
        if (!tex) {
            THROW_EXCEPTION("SDL_CreateTextureFromSurface Error: ", SDL_GetError());
        }

        SDL_SetTextureScaleMode(tex.get(), SDL_SCALEMODE_NEAREST);

        setupImGui(win.get(), ren.get());

        bool quit = false;
        SDL_Event e;


        GameWrapper gameWrapper(tex, sfxStream, bgmStream);

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
            
            startImGuiFrame();

            if (displayMenu)
            {
                displayGameSettingsWindow(renderDrivers, selectedRenderer, quit, displayMenu);
            }
            
            // ImGui::ShowDemoWindow();

            if (!displayMenu)
            {
                gameWrapper.drawFrame();
            }
            
            SDL_RenderClear(ren.get());
            SDL_FRect dst = {
                (float)screenSizeHelper.getRenderOffsetX(), (float)screenSizeHelper.getRenderOffsetY(),
                (float)screenSizeHelper.getRenderWidth(), (float)screenSizeHelper.getRenderHeight()};
            SDL_RenderTexture(ren.get(), tex.get(), nullptr, &dst);

            renderImGui(ren.get());

            SDL_RenderPresent(ren.get());

            // handle events
            while (SDL_PollEvent(&e)) {
                ImGui_ImplSDL3_ProcessEvent(&e);
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
                displayMenu = true;
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

        SDL_CloseAudioDevice(deviceId);

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
