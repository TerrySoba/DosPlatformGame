#include "music_controller_dos.h"

#include "rad_player.h"

MusicControllerDos::MusicControllerDos() :
    m_currentSong(MUSIC_INDEX_NO_MUSIC)
{
}

MusicControllerDos::~MusicControllerDos()
{
}

void MusicControllerDos::playMusic(SongIndex index)
{
    // do not reload music if it is already playing
    if (index == MUSIC_INDEX_NO_CHANGE || index == m_currentSong)
    {
        return;
    }

    m_currentSong = index;

    const char* modulePath = 0;

    switch (index)
    {
    case MUSIC_INDEX_MAIN_MUSIC:
        modulePath = "FUREXUS2.RAD";
        break;
    case MUSIC_INDEX_DARK_MUSIC:
        modulePath = "FIRST.RAD";
        break;
    case MUSIC_INDEX_BOSS_MUSIC:
        modulePath = "THIRD.RAD";
        break;
    case MUSIC_INDEX_DIGITAL_MUSIC:
        modulePath = "CELT.RAD";
        break;
    case MUSIC_INDEX_BOSS2_MUSIC:
        modulePath = "SECOND.RAD";
        break;
    case MUSIC_INDEX_STRANGE_MUSIC:
        modulePath = "STRANGE.RAD";
        break;
    case MUSIC_INDEX_INTRO_MUSIC:
        modulePath = "INTRO.RAD";
        break;
    case MUSIC_INDEX_NO_MUSIC:
        // fallthrough
    default:
        // do nothing, so that the music stops
        break;
    }

    if (modulePath)
    {
        m_player.playModule(modulePath);
    }
    else
    {
        m_player.stopModule();
    }

    return;
}