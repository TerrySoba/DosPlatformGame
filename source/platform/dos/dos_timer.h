#ifndef TIMER_H_INCLUDED
#define TIMER_H_INCLUDED

class DosTimer
{
public:
    DosTimer(void (*timerFunction)(), int frequency);
    ~DosTimer();
private:
    void __interrupt __far(*m_oldInterrupt)();
};

#endif
