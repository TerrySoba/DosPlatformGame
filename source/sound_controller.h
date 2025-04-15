#ifndef SOUND_CONTROLLER_INCLUDED_H
#define SOUND_CONTROLLER_INCLUDED_H

class SoundController
{
public:
    virtual ~SoundController() {};

    virtual void playJumpSound() = 0;
    virtual void playWalkSound() = 0;
    virtual void playGuffinSound() = 0;
    virtual void playJetpackSound() = 0;
    virtual void playDeathSound() = 0;
    virtual void playStandSound() = 0;
    virtual void playSwitchSound() = 0;
    virtual bool isPlaying() = 0;
};


#endif