#ifndef SOUND_CONTROLLER_SDL_INCLUDED_H
#define SOUND_CONTROLLER_SDL_INCLUDED_H

#include "sound_controller.h"
#include "decode_vorbis.h"

#include <SDL3/SDL_audio.h>
#include <memory>

class SoundControllerSdl : public SoundController
{
public:
    SoundControllerSdl(std::shared_ptr<SDL_AudioStream> audioStream);
    ~SoundControllerSdl();

    virtual void playJumpSound();
    virtual void playWalkSound();
    virtual void playGuffinSound();
    virtual void playJetpackSound();
    virtual void playDeathSound();
    virtual void playStandSound();
    virtual void playSwitchSound();
    virtual bool isPlaying();

private:
    void playSound(const SampleData& sound, int32_t priority);

private:

    std::shared_ptr<SDL_AudioStream> m_audioStream;

    SampleData m_jumpSound;
    SampleData m_walkSound;
    SampleData m_deathSound;
    SampleData m_standSound;
    SampleData m_guffinSound;
    SampleData m_jetpackSound;
    SampleData m_switchSound;
    uint16_t m_soundPriority;

    int32_t m_lastPriority = 0;
};

#endif