#ifndef MUSIC_CONTROLLER_INCLUDED_H
#define MUSIC_CONTROLLER_INCLUDED_H

enum SongIndex
{
    MUSIC_INDEX_NO_CHANGE = 0,   // do not change the current song, if any
    MUSIC_INDEX_NO_MUSIC = 1,    // stop playing music
    MUSIC_INDEX_TITLE_MUSIC = 2, // play title music
    MUSIC_INDEX_LEVEL_MUSIC = 3, // play level music
    MUSIC_INDEX_BOSS_MUSIC = 4,  // play boss music
    MUSIC_INDEX_ENDING_MUSIC = 5,// play ending music
};

// forward declaration
class RadPlayer;

class MusicController
{
public:
    MusicController();
    ~MusicController();

    void playMusic(SongIndex index);

private:
    SongIndex m_currentSong;
    RadPlayer* m_player;
};

#endif