#ifndef MUSIC_CONTROLLER_INCLUDED_H
#define MUSIC_CONTROLLER_INCLUDED_H

enum SongIndex
{
    MUSIC_INDEX_NO_CHANGE = 0,   // do not change the current song, if any
    MUSIC_INDEX_NO_MUSIC = 1,    // stop playing music
    MUSIC_INDEX_MAIN_MUSIC = 2,  
    MUSIC_INDEX_DARK_MUSIC = 3,
    MUSIC_INDEX_BOSS_MUSIC = 4,  
    MUSIC_INDEX_DIGITAL_MUSIC = 5,
    MUSIC_INDEX_BOSS2_MUSIC = 6,
    MUSIC_INDEX_STRANGE_MUSIC = 7,
    MUSIC_INDEX_INTRO_MUSIC = 8,
};

class MusicController
{
public:
    virtual ~MusicController() {}
    virtual void playMusic(SongIndex index) = 0;
    virtual SongIndex getCurrentSong() const = 0;
};

#endif