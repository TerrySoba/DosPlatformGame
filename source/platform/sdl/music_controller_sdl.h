#ifndef MUSIC_CONTROLLER_SDL_INCLUDED_H
#define MUSIC_CONTROLLER_SDL_INCLUDED_H

#include "music_controller.h"


class MusicControllerSdl : public MusicController
{
public:
    ~MusicControllerSdl() {}

    void playMusic(SongIndex index) {}

    SongIndex getCurrentSong() const { return MUSIC_INDEX_NO_MUSIC; }
};

#endif