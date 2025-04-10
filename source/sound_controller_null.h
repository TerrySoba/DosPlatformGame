#ifndef SOUND_CONTROLLER_NULL_INCLUDED_H
#define SOUND_CONTROLLER_NULL_INCLUDED_H

class SoundController
{
public:

    void playJumpSound() {}
    void playWalkSound() {}
    void playGuffinSound() {}
    void playJetpackSound() {}
    void playDeathSound() {}
    void playStandSound() {}
    void playSwitchSound() {}

    bool isPlaying()
    {
        return false;
    }
};


#endif