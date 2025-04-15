#ifndef SOUND_CONTROLLER_DOS_INCLUDED_H
#define SOUND_CONTROLLER_DOS_INCLUDED_H

#include "platform/dos/soundblaster.h"

#include "sound_controller.h"

class SoundControllerDos : public SoundController
{
public:
    SoundControllerDos();
    ~SoundControllerDos();

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