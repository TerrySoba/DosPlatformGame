#ifndef SOUND_CONTROLLER_INCLUDED_H
#define SOUND_CONTROLLER_INCLUDED_H

#include "soundblaster.h"

class SoundController
{
public:
    SoundController();

    void playJumpSound();
    void playWalkSound();
    void playDeathSound();
    void playStandSound();

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
    uint16_t m_soundPriority;
};


#endif