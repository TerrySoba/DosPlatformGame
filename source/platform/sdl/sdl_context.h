#ifndef SDL_CONTEXT_H
#define SDL_CONTEXT_H

#include <SDL3/SDL.h>

#include <vector>
#include <string>
#include <memory>

class SdlContext
{
public:
    /**
     * Initializes SDL audio and video.
     * 
     * The destructor de-initializes SDL.
     */
    SdlContext(
        bool fullscreen,
        int gameWindowResolutionWidth,
        int gameWindowResolutionHeight,
        std::string sdlRenderer);
    ~SdlContext() = default;

    std::shared_ptr<SDL_AudioStream> getBgmStream() { return m_bgmStream; }
    std::shared_ptr<SDL_AudioStream> getSfxStream() { return m_sfxStream; }
    std::shared_ptr<SDL_Window> getWindow() { return m_window; }
    std::shared_ptr<SDL_Renderer> getRenderer() { return m_renderer; }
    std::shared_ptr<SDL_Texture> getTexture() { return m_texture; }

public: // static methods
    static std::vector<std::string> getAvailableRenderDrivers();

private: // methods
    void initializeAudio();
    void initializeVideo(
        bool fullscreen,
        int gameWindowResolutionWidth,
        int gameWindowResolutionHeight,
        std::string sdlRenderer);

private: // member variables
    std::shared_ptr<void> m_sdlHolder;
    std::shared_ptr<void> m_audioDeviceHolder;
    std::shared_ptr<SDL_AudioStream> m_bgmStream;
    std::shared_ptr<SDL_AudioStream> m_sfxStream;
    std::shared_ptr<SDL_Window> m_window;
    std::shared_ptr<SDL_Renderer> m_renderer;
    std::shared_ptr<SDL_Texture> m_texture;
};

#endif