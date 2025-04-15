#ifndef SOUND_CONTROLLER_SDL_INCLUDED_H
#define SOUND_CONTROLLER_SDL_INCLUDED_H

#include "sound_controller.h"

class SoundControllerSdl : public SoundController
{
public:
    virtual void playJumpSound() {};
    virtual void playWalkSound() {};
    virtual void playGuffinSound() {};
    virtual void playJetpackSound() {};
    virtual void playDeathSound() {};
    virtual void playStandSound() {};
    virtual void playSwitchSound() {};
    virtual bool isPlaying() { return false;}
};


#endif