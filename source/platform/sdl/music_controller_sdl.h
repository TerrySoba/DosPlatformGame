#ifndef MUSIC_CONTROLLER_SDL_INCLUDED_H
#define MUSIC_CONTROLLER_SDL_INCLUDED_H

#include "music_controller.h"

#include <thread>
#include <mutex>
#include <memory>

#include <SDL3/SDL.h>


#include "decode_vorbis.h"

class MusicControllerSdl : public MusicController
{
public:
    MusicControllerSdl(std::shared_ptr<SDL_AudioStream> audioStream);
    ~MusicControllerSdl();

    // void handleAudioStreamCallback(SDL_AudioStream* stream, void* userdata, uint8_t* streamBuffer, int32_t length); 

    // MusicController interface
    void playMusic(SongIndex index);
    SongIndex getCurrentSong() const;

    void sdlFillAudioStream(SDL_AudioStream* stream, int additional_amount, int total_amount);


private:
    std::mutex m_mutex;
    std::shared_ptr<SDL_AudioStream> m_audioStream;
    std::shared_ptr<VorbisDecoder> m_currentSongDecoder;
    std::vector<int16_t> m_musicBuffer;

    SongIndex m_currentSong = MUSIC_INDEX_NO_CHANGE;
};

#endif