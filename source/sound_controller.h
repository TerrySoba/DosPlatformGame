#ifndef SOUND_CONTROLLER_INCLUDED_H
#define SOUND_CONTROLLER_INCLUDED_H

#include "platform/dos/soundblaster.h"

class SoundController
{
public:
    SoundController();
    ~SoundController();

    void playJumpSound();
    void playWalkSound();
    void playGuffinSound();
    void playJetpackSound();
    void playDeathSound();
    void playStandSound();
    void playSwitchSound();

    bool isPlaying()
    {
        return m_sb.isPlaying();
    }

private:
    SoundBlaster m_sb;
    bool m_sbFound;
    SbSample m_jumpSound;
    SbSample m_walkSound;
    SbSample m_deathSound;
    SbSample m_standSound;
    SbSample m_guffinSound;
    SbSample m_jetpackSound;
    SbSample m_switchSound;
    uint16_t m_soundPriority;
};


#endif