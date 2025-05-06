#include "sound_controller_sdl.h"

SoundControllerSdl::SoundControllerSdl(std::shared_ptr<SDL_AudioStream> audioStream) :
    m_audioStream(audioStream)
{
    m_jumpSound = decodeOpusFile("jump.opus");
    m_walkSound = decodeOpusFile("steps.opus");
    m_deathSound = decodeOpusFile("death.opus");
    m_standSound = decodeOpusFile("stand.opus");
    m_guffinSound = decodeOpusFile("guffin.opus");
    m_jetpackSound = decodeOpusFile("jetpack.opus");
    m_switchSound = decodeOpusFile("switch2.opus");
}

SoundControllerSdl::~SoundControllerSdl()
{
    // Cleanup code if needed
}

void SoundControllerSdl::playJumpSound() {};
void SoundControllerSdl::playWalkSound() {};

void SoundControllerSdl::playGuffinSound()
{
    
};

void SoundControllerSdl::playJetpackSound() {};
void SoundControllerSdl::playDeathSound() {};
void SoundControllerSdl::playStandSound() {};
void SoundControllerSdl::playSwitchSound() {};
bool SoundControllerSdl::isPlaying() { return false;}
