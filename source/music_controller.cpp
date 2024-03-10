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
    // return;
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
    case MUSIC_INDEX_NO_MUSIC:
        // do nothing, so that the music stops
        break;
    case MUSIC_INDEX_TITLE_MUSIC:
        modulePath = "FUREXUS2.RAD";
        break;
    case MUSIC_INDEX_LEVEL_MUSIC:
        modulePath = "FUREXUS2.RAD";
        break;
    case MUSIC_INDEX_BOSS_MUSIC:
        modulePath = "THIRD.RAD";
        break;
    }

    if (modulePath)
    {
        m_player = new RadPlayer(modulePath);
    }
}