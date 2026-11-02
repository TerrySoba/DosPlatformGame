#ifndef TIMER_H_INCLUDED
#define TIMER_H_INCLUDED

class DosTimer
{
public:
    DosTimer(void (*timerFunction)(), int frequency);
    ~DosTimer();
};

#endif
