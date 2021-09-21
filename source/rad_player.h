#ifndef RAD_PLAYER_H_INCLUDED
#define RAD_PLAYER_H_INCLUDED

#include "timer.h"

class RadPlayer
{
public:
    RadPlayer(const char* modulePath);
    ~RadPlayer();

private:
    DosTimer* m_timer;
    void* m_songData;
};

#endif
