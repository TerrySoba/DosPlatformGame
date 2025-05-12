#include "sound_controller_sdl.h"

#include <SDL3/SDL.h>

SoundControllerSdl::SoundControllerSdl(std::shared_ptr<SDL_AudioStream> audioStream) :
    m_audioStream(audioStream)
{
    m_jumpSound = decodeVorbisFile("jump.ogg");
    m_walkSound = decodeVorbisFile("steps.ogg");
    m_deathSound = decodeVorbisFile("death.ogg");
    m_standSound = decodeVorbisFile("stand.ogg");
    m_guffinSound = decodeVorbisFile("guffin.ogg");
    m_jetpackSound = decodeVorbisFile("jetpack.ogg");
    m_switchSound = decodeVorbisFile("switch2.ogg");
}

SoundControllerSdl::~SoundControllerSdl()
{
    // Cleanup code if needed
}

void SoundControllerSdl::playJumpSound()
{
    playSound(m_jumpSound, 2);
};

void SoundControllerSdl::playWalkSound()
{
    playSound(m_walkSound, 1);
};

void SoundControllerSdl::playGuffinSound()
{
    playSound(m_guffinSound, 3);
};

void SoundControllerSdl::playJetpackSound()
{
    playSound(m_jetpackSound, 4);
};

void SoundControllerSdl::playDeathSound()
{
    playSound(m_deathSound, 4);
};

void SoundControllerSdl::playStandSound()
{
    playSound(m_standSound, 1);
};

void SoundControllerSdl::playSwitchSound()
{
    playSound(m_switchSound, 5);
};

bool SoundControllerSdl::isPlaying()
{
    auto queueSize = SDL_GetAudioStreamQueued(m_audioStream.get());
    if (queueSize > 0)
    {
        return true;
    } else {
        return false;
    }
}

void SoundControllerSdl::playSound(const SampleData& sound, int32_t priority)
{
    if (isPlaying())
    {
        if (priority <= m_lastPriority)
        {
            return;
        }

        SDL_ClearAudioStream(m_audioStream.get());
    }

    if (!SDL_PutAudioStreamData(m_audioStream.get(), sound.data(), sound.size() * sizeof(SampleData::value_type)))
    {
        SDL_LogError(1, "SDL_PutAudioStreamData Error: %s", SDL_GetError());
    }

    m_lastPriority = priority;
}
