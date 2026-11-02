#include "dos_timer.h"

#include <dos.h>
#include <stdint.h>
#include <conio.h>

#define TIMER_INTERRUPT 0x08

// Number of PIT input clocks between two calls of the original BIOS timer handler (18.2 Hz).
// The 16 bit tick accumulator wraps around exactly after this many clocks.
#define BIOS_TIMER_PERIOD 0x10000ul

// Stack used while the timer function runs. The interrupt can happen while DOS or the BIOS
// is running on a small internal stack, so we must not put the timer function's stack usage
// on the interrupted stack. Interrupts stay disabled while it is in use, so one stack is enough.
#define ISR_STACK_SIZE 512

// Watcom's stack overflow check compares SP with the limits of the program's own stack.
// Inside the interrupt SS:SP may point to any stack (e.g. the one of DOS), the check would
// fail and abort the program from within the interrupt, which hangs the machine.
// So no code in this file may contain stack checks.
#pragma off (check_stack)

static void (*s_timerFunction)();
static void __interrupt __far (*s_oldInterrupt)();

static uint16_t s_timerDivisor;
static uint16_t s_biosTickAccumulator;

static uint8_t s_isrStack[ISR_STACK_SIZE];
static uint16_t s_isrStackSeg;
static uint16_t s_isrStackTop;
static uint16_t s_savedSs;
static uint16_t s_savedSp;


static void __interrupt __far customTimerInterrupt()
{
    // switch to our own stack
    __asm {
        // The compiler places this block before it loads DS, so load it here.
        mov ax, seg s_savedSs
        mov ds, ax
        mov s_savedSs, ss
        mov s_savedSp, sp
        mov ss, s_isrStackSeg
        mov sp, s_isrStackTop
    }

    s_timerFunction();

    // back to the interrupted stack
    __asm {
        mov ss, s_savedSs
        mov sp, s_savedSp
    }

    // Keep the BIOS handler running at its normal rate of 18.2 Hz, so that the
    // DOS time and the BIOS timeouts (e.g. floppy motor off) keep working.
    s_biosTickAccumulator += s_timerDivisor;
    if (s_biosTickAccumulator < s_timerDivisor)
    {
        // the BIOS handler signals the end of the interrupt itself
        s_oldInterrupt();
    }
    else
    {
        // signal interrupt done
        __asm{
            "mov al,20H"
            "out 20H,al"
        }
    }
}

/**
 * Only one DosTimer may exist at a time.
 */
DosTimer::DosTimer(void (*timerFunction)(), int frequency)
{
    s_timerFunction = timerFunction;

    uint8_t __far* stackTop = s_isrStack + ISR_STACK_SIZE;
    s_isrStackSeg = FP_SEG(stackTop);
    s_isrStackTop = FP_OFF(stackTop);

    // The clock we're dealing with here runs at 1.193182mhz, so we
    // just divide 1.193182 by the number of triggers we want per
    // second to get our divisor.
    uint32_t c = 1193182 / (uint32_t)frequency;
    uint16_t speed = c;

    s_timerDivisor = speed;
    s_biosTickAccumulator = 0;

    // install the handler before speeding up the timer
    s_oldInterrupt = _dos_getvect(TIMER_INTERRUPT);
    _dos_setvect(TIMER_INTERRUPT, customTimerInterrupt);

    // set clock speed
    __asm {
        cli
      	mov     bx,  speed //  set the clock speed to selected frequency
      	mov     al,  00110110b
    	out     43h, al
      	mov     al,  bl
	    out     40h, al
    	mov     al,  bh
      	out     40h, al
        sti
    }
}

DosTimer::~DosTimer()
{
    __asm {
        cli
      	xor bx,  bx        // min rate 18.2 Hz when set to zero
    	mov al,  00110110b
      	out 43h, al
	    mov al,  bl
    	out 40h, al
      	mov al,  bh
	    out 40h, al
        sti
    }

    _dos_setvect(TIMER_INTERRUPT, s_oldInterrupt);
}
