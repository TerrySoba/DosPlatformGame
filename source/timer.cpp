#include "timer.h"

#include <dos.h>
#include <stdint.h>
#include <conio.h>

#define TIMER_INTERRUPT 0x08

static void (*s_timerFunction)();


static void __interrupt __far customTimerInterrupt()
{
    s_timerFunction();

    // signal interrupt done
    __asm{
        "mov al,20H"
        "out 20H,al"
    }
}

DosTimer::DosTimer(void (*timerFunction)(), int frequency)
{
    s_timerFunction = timerFunction;

    // The clock we're dealing with here runs at 1.193182mhz, so we
    // just divide 1.193182 by the number of triggers we want per
    // second to get our divisor.
    uint32_t c = 1193182 / (uint32_t)frequency;
    uint16_t speed = c;

    // set clock speed
    __asm {
        cli
      	mov     bx,  speed //  set the clock speed to 60Hz (1193180/60)
      	mov     al,  00110110b
    	out     43h, al
      	mov     al,  bl
	    out     40h, al
    	mov     al,  bh
      	out     40h, al
        sti
    }

    m_oldInterrupt = _dos_getvect(TIMER_INTERRUPT);
    _dos_setvect(TIMER_INTERRUPT, customTimerInterrupt);
}

DosTimer::~DosTimer()
{
    __asm {
      	xor bx,  bx        // min rate 18.2 Hz when set to zero
    	mov al,  00110110b
      	out 43h, al
	    mov al,  bl
    	out 40h, al
      	mov al,  bh
	    out 40h, al
    }

    _dos_setvect(TIMER_INTERRUPT, m_oldInterrupt);
}
