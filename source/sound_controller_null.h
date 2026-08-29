#ifndef SOUND_CONTROLLER_NULL_INCLUDED_H
#define SOUND_CONTROLLER_NULL_INCLUDED_H

#include "sound_controller.h"

class SoundControllerNull : public SoundController
{
public:

    virtual ~SoundControllerNull() {};

    virtual void playJumpSound() {}
    virtual void playWalkSound() {}
    virtual void playGuffinSound() {}
    virtual void playJetpackSound() {}
    virtual void playDeathSound() {}
    virtual void playStandSound() {}
    virtual void playSwitchSound() {}
    virtual void playPortalSound() {}
    virtual bool isPlaying() { return false; }
};

#endif