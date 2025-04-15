#ifndef MUSIC_CONTROLLER_DOS_INCLUDED_H
#define MUSIC_CONTROLLER_DOS_INCLUDED_H

#include "music_controller.h"

#include "rad_player.h"


class MusicControllerDos : public MusicController
{
public:
    MusicControllerDos();
    ~MusicControllerDos();

    void playMusic(SongIndex index);

    SongIndex getCurrentSong() const { return m_currentSong; }

private:
    SongIndex m_currentSong;
    RadPlayer m_player;
};

#endif