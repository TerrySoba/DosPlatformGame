#include "music_controller.h"

#include "rad_player.h"

MusicController::MusicController() :
    m_currentSong(MUSIC_INDEX_NO_MUSIC),
    m_player(0)
{
}

MusicController::~MusicController()
{
    if (m_player)
    {
        delete m_player;
        m_player = 0;
    }
}

void MusicController::playMusic(SongIndex index)
{
    // do not reload music if it is already playing
    if (index == MUSIC_INDEX_NO_CHANGE || index == m_currentSong)
    {
        return;
    }

    m_currentSong = index;

    if (m_player)
    {
        delete m_player;
        m_player = 0;
    }

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
    case MUSIC_INDEX_NO_MUSIC:
        // fallthrough
    default:
        // do nothing, so that the music stops
        break;
    }

    if (modulePath)
    {
        m_player = new RadPlayer(modulePath);
    }

    return;
}