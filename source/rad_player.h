#ifndef RAD_PLAYER_H_INCLUDED
#define RAD_PLAYER_H_INCLUDED

#include "platform/dos/dos_timer.h"
#include "stdint.h"

#include "shared_ptr.h"

class RadPlayer
{
public:
    RadPlayer();
    ~RadPlayer();

    void playModule(const char* filename);
    void stopModule();

private:
    void radLoadModuleInternal(const char* filename);

private:
    tnd::shared_ptr<DosTimer> m_timer;
    void* m_songDataOrigBlock;
    void* m_songData;
    uint16_t m_songDataSize;
};

#endif
