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

private:
    SoundBlaster m_sb;
    bool m_sbFound;
    SbSample m_jumpSound;
    SbSample m_walkSound;
    SbSample m_deathSound;

};


#endif