#include "music_controller_sdl.h"

void SDLCALL fillAudioStream(void *userdata, SDL_AudioStream *stream, int additional_amount, int total_amount)
{
    // SDL_Log("Audio stream callback called with additional amount: %d, total amount: %d", additional_amount, total_amount);
    
}

MusicControllerSdl::MusicControllerSdl(std::shared_ptr<SDL_AudioStream> audioStream) :
    m_audioStream(audioStream)
{
    
    SDL_SetAudioStreamGetCallback(audioStream.get(), fillAudioStream, nullptr);
}

MusicControllerSdl::~MusicControllerSdl()
{
    SDL_SetAudioStreamGetCallback(m_audioStream.get(), nullptr, nullptr);
}

void MusicControllerSdl::playMusic(SongIndex index) {}

SongIndex MusicControllerSdl::getCurrentSong() const
{
    return MUSIC_INDEX_NO_MUSIC;
}
