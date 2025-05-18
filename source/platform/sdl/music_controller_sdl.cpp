#include "music_controller_sdl.h"


void SDLCALL fillAudioStream(void *userdata, SDL_AudioStream *stream, int additional_amount, int total_amount)
{
    MusicControllerSdl* musicController = static_cast<MusicControllerSdl*>(userdata);
    musicController->sdlFillAudioStream(stream, additional_amount, total_amount);
}

void MusicControllerSdl::sdlFillAudioStream(SDL_AudioStream* stream, int additional_amount, int total_amount)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (!m_currentSongDecoder)
    {
        return;
    }

    int queueSize = SDL_GetAudioStreamQueued(stream);
    constexpr int halfSecond = (48000 * sizeof(int16_t) * 2) / 2;
    while (queueSize < halfSecond)
    {
        uint32_t decodedSamples = m_currentSongDecoder->decode(m_musicBuffer.data(), m_musicBuffer.size());
        if (decodedSamples == 0)
        {
            m_currentSongDecoder->rewind();
        }
        SDL_PutAudioStreamData(stream, m_musicBuffer.data(), decodedSamples * sizeof(int16_t) * 2);
        queueSize = SDL_GetAudioStreamQueued(stream);
    }
}

MusicControllerSdl::MusicControllerSdl(std::shared_ptr<SDL_AudioStream> audioStream) :
    m_audioStream(audioStream),
    m_musicBuffer(1024 * 2) // 1024 samples for stereo (2 channels)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_currentSongDecoder = std::make_shared<VorbisDecoder>("celt.ogg");

    SDL_SetAudioStreamGetCallback(audioStream.get(), fillAudioStream, this);
}

MusicControllerSdl::~MusicControllerSdl()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    SDL_SetAudioStreamGetCallback(m_audioStream.get(), nullptr, nullptr);
}

void MusicControllerSdl::playMusic(SongIndex index)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (index == MUSIC_INDEX_NO_CHANGE)
    {
        return;
    }

    if (index == getCurrentSong())
    {
        return;
    }

    m_currentSong = index;

    switch (index)
    {
        case MUSIC_INDEX_NO_MUSIC:
            m_currentSongDecoder.reset();
            break;
        case MUSIC_INDEX_MAIN_MUSIC:
            m_currentSongDecoder = std::make_shared<VorbisDecoder>("furexus.ogg");
            break;
        case MUSIC_INDEX_DARK_MUSIC:
            m_currentSongDecoder = std::make_shared<VorbisDecoder>("first.ogg");
            break;
        case MUSIC_INDEX_BOSS_MUSIC:
            m_currentSongDecoder = std::make_shared<VorbisDecoder>("third.ogg");
            break;
        case MUSIC_INDEX_DIGITAL_MUSIC:
            m_currentSongDecoder = std::make_shared<VorbisDecoder>("celt.ogg");
            break;
        case MUSIC_INDEX_BOSS2_MUSIC:
            m_currentSongDecoder = std::make_shared<VorbisDecoder>("second.ogg");
            break;
        case MUSIC_INDEX_STRANGE_MUSIC:
            m_currentSongDecoder = std::make_shared<VorbisDecoder>("strange.ogg");
            break;
        case MUSIC_INDEX_INTRO_MUSIC:
            m_currentSongDecoder = std::make_shared<VorbisDecoder>("intro.ogg");
            break;
    }
}

SongIndex MusicControllerSdl::getCurrentSong() const
{
    return m_currentSong;
}
