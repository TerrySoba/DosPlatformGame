#include "joystick.h"

#include <stdio.h>
#include <conio.h>

uint16_t s_jsDeadzoneXMin = 0;
uint16_t s_jsDeadzoneXMax = 0;
uint16_t s_jsDeadzoneYMin = 0;
uint16_t s_jsDeadzoneYMax = 0;

bool s_joystickConnected = false;

struct RawJoystickState
{
    uint8_t buttons;
    uint16_t x1, y1, x2, y2;
};

RawJoystickState readJoystickRaw()
{
    uint8_t buttons = 0;
    uint16_t x1 = 0;
    uint16_t y1 = 0;

    // The joystick is read by writing a byte of data to port 0x201, the actual
    // value does not matter.
    // Then periodically read port 0x201 until the bits 0-3 of the read value
    // become 0. The waiting time until the bits become 0 corresponds to the
    // analogue value of the joystick axis. Because the timing needs to be
    // accurate, interrups are disabled using "cli".
    //
    // There is also a BIOS interrupt that may be use to read joystick values,
    // but this code seems to be faster. At least on an IBM PS/2 Model 30 286.

    __asm{cli} // disable interrupts
    outp(0x201, 1); // start analogue joystick value acquisition
    uint16_t counter = 0;
    for(counter = 0; counter < 0xffff; ++counter)
    {
        buttons = inp(0x201); // read current joystick status

        // The single "&" operator in the next two lines is intentional to
        // prevent short-circuit operation of "&&"" operator.
        // This would change the timing and would skew the results.
        if (((buttons & 1) == 0) & x1 == 0) x1 = counter; // check if x-axis has finished
        if (((buttons & 2) == 0) & y1 == 0) y1 = counter; // check if y-axis has finished
        if (x1 != 0 && y1 != 0) break;
    }
    __asm{sti} // re-enable interrupts

    RawJoystickState status;
    status.buttons = buttons;
    status.x1 = x1;
    status.y1 = y1;
    status.x2 = 0;
    status.y2 = 0;

    return status;
}


extern void disableCursor();
#pragma aux disableCursor =    \
    "mov ah, 01h"              \
    "mov ch, 3Fh"              \
    "int 10h"                  \
    modify [ah ch];


extern void setCursor(uint8_t row, uint8_t col);
#pragma aux setCursor =   \
        "mov ah, 02h" \
		"mov bh, 00h" \
		"int 10h" \
        modify[ah bh dx] \
        parm [dh] [dl];

uint8_t readJoystick()
{
    if (!s_joystickConnected) return 0;
    RawJoystickState s = readJoystickRaw();
    return (((s.x1 < s_jsDeadzoneXMin)?JOY_LEFT:0) |
            ((s.x1 > s_jsDeadzoneXMax)?JOY_RIGHT:0) |
            ((s.y1 < s_jsDeadzoneYMin)?JOY_UP:0) |
            ((s.y1 > s_jsDeadzoneYMax)?JOY_DOWN:0) |
            ( ((~s.buttons) & (1 << 4))?JOY_BUTTON_1:0 ) );
}

enum CalibrationState
{
    FIND_MIN_MAX,
    WAIT_FOR_BUTTON_UP1,
    FIND_CENTER,
    WAIT_FOR_BUTTON_UP2,
    FINAL,
    WAIT_FOR_BUTTON_UP3,
};


void calibrateJoystick()
{
    CalibrationState state = FIND_MIN_MAX;

    RawJoystickState s = readJoystickRaw();

    // if joystick is not connected do not execute calibration
    if (s.x1 == 0 && s.y1 == 0)
    {
        return;
    }

    // initialize with current value
    uint16_t jsCenterX = s.x1;
    uint16_t jsCenterY = s.y1;
    uint16_t jsMinX = s.x1;
    uint16_t jsMinY = s.y1;
    uint16_t jsMaxX = s.x1;
    uint16_t jsMaxY = s.y1;

    disableCursor();

    puts("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");

    setCursor(0,0);
    puts("Calibrate Joystick");
    setCursor(1,0);
    puts("������������������");
    setCursor(19,0);
    puts("Press \"Q\" on keyboard to disable joystick support.");
    setCursor(20,0);
    puts("Press \"R\" on keyboard to restart calibration.");
    setCursor(9,6);

    bool done = false;

    while (!done)
    {
        if (kbhit())
        {
            char ch = getch();
            if (ch == 'q') done = true;
            if (ch == 'r') state = FIND_MIN_MAX;
        }
        RawJoystickState s = readJoystickRaw();

        char buf[8];

        setCursor(6,3);
        sprintf(buf, "x:%05hu", s.x1);
        puts(buf);
        setCursor(7,3);
        sprintf(buf, "y:%05hu", s.y1);
        puts(buf);

        #define BUTTON_PRESSED() ((~s.buttons) & (1 << 4))

        setCursor(4,0);
        switch(state)
        {
            case FIND_MIN_MAX:
                puts("Rotate joystick, then press first joystick button.           ");
                if (s.x1 > jsMaxX) jsMaxX = s.x1;
                if (s.y1 > jsMaxY) jsMaxY = s.y1;
                if (s.x1 < jsMinX) jsMinX = s.x1;
                if (s.y1 < jsMinY) jsMinY = s.y1;
                if (BUTTON_PRESSED()) state = WAIT_FOR_BUTTON_UP1;
                break;
            case WAIT_FOR_BUTTON_UP1:
                if (!BUTTON_PRESSED()) state = FIND_CENTER;
                break;
            case FIND_CENTER:
                puts("Return joystick to center, then press first joystick button. ");
                if (BUTTON_PRESSED())
                {
                    state = WAIT_FOR_BUTTON_UP2;
                    jsCenterX = s.x1;
                    jsCenterY = s.y1;

                    #define DEADZONE_FACTOR 0.2

                    s_jsDeadzoneXMin = jsCenterX - (jsMaxX - jsMinX) * DEADZONE_FACTOR;
                    s_jsDeadzoneXMax = jsCenterX + (jsMaxX - jsMinX) * DEADZONE_FACTOR;
                    s_jsDeadzoneYMin = jsCenterY - (jsMaxY - jsMinY) * DEADZONE_FACTOR;
                    s_jsDeadzoneYMax = jsCenterY + (jsMaxY - jsMinY) * DEADZONE_FACTOR;
                    s_joystickConnected = true;
                }
                break;
            case WAIT_FOR_BUTTON_UP2:
                if (!BUTTON_PRESSED()) state = FINAL;
                break;
            case FINAL:
                puts("Press first joystick button to start game.                   ");
                if (BUTTON_PRESSED()) state = WAIT_FOR_BUTTON_UP3;
                break;
            case WAIT_FOR_BUTTON_UP3:
                if (!BUTTON_PRESSED()) done = true;
                break;
        }

        uint8_t js = readJoystick();
        setCursor(10,3);
        if (js & JOY_LEFT) puts("left");
        else puts("    ");
        setCursor(10,11);
        if (js & JOY_RIGHT) puts("right");
        else puts("     ");
        setCursor(9,8);
        if (js & JOY_UP) puts("up");
        else puts("  ");
        setCursor(11,7);
        if (js & JOY_DOWN) puts("down");
        else puts("    ");
       
    }
}
