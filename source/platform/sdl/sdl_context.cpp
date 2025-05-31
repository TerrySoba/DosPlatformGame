#include "sdl_context.h"

#include "exception.h"

void SdlContext::initializeAudio()
{
    SDL_AudioSpec spec;
    spec.freq = 48000;
    spec.format = SDL_AUDIO_S16;
    spec.channels = 2;

    SDL_AudioDeviceID deviceId = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec);
    if (deviceId == 0)
    {
        SDL_LogError(1, "SDL_OpenAudioDevice Error: %s", SDL_GetError());
        THROW_EXCEPTION("SDL_OpenAudioDevice Error: ", SDL_GetError());
    }
    m_audioDeviceHolder = std::shared_ptr<void>(nullptr, [deviceId](void*){ SDL_CloseAudioDevice(deviceId); });

    SDL_ResumeAudioDevice(deviceId);

    m_bgmStream = std::shared_ptr<SDL_AudioStream>(SDL_CreateAudioStream(&spec, &spec), SDL_DestroyAudioStream);
    if (!m_bgmStream)
    {
        SDL_LogError(1, "SDL_CreateAudioStream Error: %s", SDL_GetError());
        THROW_EXCEPTION("SDL_CreateAudioStream Error: ", SDL_GetError());
    }

    if (!SDL_BindAudioStream(deviceId, m_bgmStream.get()))
    {
        SDL_LogError(1, "SDL_BindAudioStream Error: %s", SDL_GetError());
        THROW_EXCEPTION("SDL_BindAudioStream Error: ", SDL_GetError());
    }

    m_sfxStream = std::shared_ptr<SDL_AudioStream>(SDL_CreateAudioStream(&spec, &spec), SDL_DestroyAudioStream);
    if (!m_sfxStream)
    {
        SDL_LogError(1, "SDL_CreateAudioStream Error: %s", SDL_GetError());
        THROW_EXCEPTION("SDL_CreateAudioStream Error: ", SDL_GetError());
    }

    if (!SDL_BindAudioStream(deviceId, m_sfxStream.get()))
    {
        SDL_LogError(1, "SDL_BindAudioStream Error: %s", SDL_GetError());
        THROW_EXCEPTION("SDL_BindAudioStream Error: ", SDL_GetError());
    }
}

void SdlContext::initializeVideo(
    bool fullscreen,
    int gameWindowResolutionWidth,
    int gameWindowResolutionHeight,
    std::string sdlRenderer)
{
    SDL_WindowFlags windowFlags = 0;
    if (fullscreen)
    {
        windowFlags |= SDL_WINDOW_FULLSCREEN;
        SDL_HideCursor();
    }

    m_window = std::shared_ptr<SDL_Window>(SDL_CreateWindow("SdlPlatformGame", gameWindowResolutionWidth, gameWindowResolutionHeight, windowFlags), SDL_DestroyWindow);
    if (!m_window)
    {
        THROW_EXCEPTION("SDL_CreateWindow Error: ", SDL_GetError());
    }

    const char *rendererName = sdlRenderer.empty() ? nullptr : sdlRenderer.c_str();

    m_renderer = std::shared_ptr<SDL_Renderer> (SDL_CreateRenderer(m_window.get(), rendererName), SDL_DestroyRenderer);
    if (!m_renderer)
    {
        THROW_EXCEPTION("SDL_CreateRenderer Error: ", SDL_GetError());
    }

    // get name of renderer
    std::string selectedRenderer = SDL_GetRendererName(m_renderer.get());

    m_texture = std::shared_ptr<SDL_Texture>(SDL_CreateTexture(m_renderer.get(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 320, 200), SDL_DestroyTexture);
    if (!m_texture)
    {
        THROW_EXCEPTION("SDL_CreateTextureFromSurface Error: ", SDL_GetError());
    }

    SDL_SetTextureScaleMode(m_texture.get(), SDL_SCALEMODE_NEAREST);
}

std::vector<std::string> SdlContext::getAvailableRenderDrivers()
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

void getSupportedResolutions()
{
    // // list available resolutions
    // int num_displays;
    // SDL_DisplayID* displays = SDL_GetDisplays(&num_displays);

    // std::vector<SDL_DisplayID> displayIDs;
    // for (int i = 0; i < num_displays; ++i)
    // {
    //     displayIDs.push_back(displays[i]);
    // }
    // SDL_free(displays);

    // SDL_Log("Found %d display(s)", num_displays);

    // for (auto id : displayIDs)
    // {
    //     int count;
    //     SDL_DisplayMode** modes = SDL_GetFullscreenDisplayModes(id, &count);

    //     for (int i = 0; i < count; ++i)
    //     {
    //         SDL_Log("Screen: %dx%d@%.1fHz", modes[i]->w, modes[i]->h, modes[i]->refresh_rate);
    //     }
    //     SDL_free(modes);
    // }

    // 
}

SdlContext::SdlContext(
    bool fullscreen,
    int gameWindowResolutionWidth,
    int gameWindowResolutionHeight,
    std::string sdlRenderer)
{
    SDL_Log("Initializing SDL.");

    if (!SDL_InitSubSystem(SDL_INIT_VIDEO | SDL_INIT_AUDIO))
    {
        THROW_EXCEPTION("SDL_InitSubSystem Error: ", SDL_GetError());
    }

    m_sdlHolder = std::shared_ptr<void>(nullptr, [](void*){SDL_Log("Shutting down SDL."); SDL_Quit();});

    initializeAudio();
    initializeVideo(fullscreen, gameWindowResolutionWidth, gameWindowResolutionHeight, sdlRenderer);

    // const char *videoDriver = SDL_GetCurrentVideoDriver();
    // if (videoDriver)
    // {
    //     SDL_Log("Current video driver: %s", videoDriver);
    // }
    // else
    // {
    //     SDL_Log("Failed to get video driver.");
    // }
}
