#ifndef MUSIC_CONTROLLER_SDL_INCLUDED_H
#define MUSIC_CONTROLLER_SDL_INCLUDED_H

#include "music_controller.h"

#include <thread>
#include <memory>

#include <SDL3/SDL.h>

class MusicControllerSdl : public MusicController
{
public:
    MusicControllerSdl(std::shared_ptr<SDL_AudioStream> audioStream);
    ~MusicControllerSdl();

    // void handleAudioStreamCallback(SDL_AudioStream* stream, void* userdata, uint8_t* streamBuffer, int32_t length); 

    // MusicController interface
    void playMusic(SongIndex index);
    SongIndex getCurrentSong() const;

private:
    std::mutex m_mutex;
    std::shared_ptr<SDL_AudioStream> m_audioStream;
};

#endif