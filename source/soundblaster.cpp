#include "soundblaster.h"

#include <dos.h>
#include <conio.h>
#include <stdlib.h>

#define SB_RESET 0x6
#define SB_READ_DATA 0xA
#define SB_READ_DATA_STATUS 0xE

bool reset_dsp(uint16_t port)
{
    outp( port + SB_RESET, 1 );
    delay(10);
    outp( port + SB_RESET, 0 );
    delay(10);
    if( ((inp(port + SB_READ_DATA_STATUS) & 0x80) == 0x80) &&
         (inp(port + SB_READ_DATA) == 0x0AA )) 
    {
        return true;
    }
    return false;
}

uint16_t readInteger(const char* number, int& len, int base)
{
    len = 0;
    if (!number) return 0;

    const int bufferSize = 8;

    char buffer[bufferSize];    
    while (*number && *number >= '0' && *number <= '9' && len < bufferSize)
    {
        buffer[len] = *number;
        ++len;
        ++number;
    }
    buffer[len] = 0;

    return strtol(buffer, NULL, base);
}

SbConfig parseBlasterString()
{
    SbConfig config = {0,0,0,SOUNDBLASTER_UNKNOWN};
    const char* blasterEnv = getenv("BLASTER");

    if (blasterEnv)
    {
        while (char ch = *blasterEnv)
        {
            int numLen;
            switch(ch)
            {
                case 'A':
                    config.base = readInteger(blasterEnv+1, numLen, 16);
                    blasterEnv += numLen;
                    break;
                case 'I':
                    config.irq = readInteger(blasterEnv+1, numLen, 10);
                    blasterEnv += numLen;
                    break;
                case 'D':
                    config.dma = readInteger(blasterEnv+1, numLen, 10);
                    blasterEnv += numLen;
                    break;
                case 'T':
                    config.type = (SoundblasterType)readInteger(blasterEnv+1, numLen, 10);
                    blasterEnv += numLen;
                    break;
            }
            ++blasterEnv;
        }
    }

    return config;
}

bool sb_detect(SbConfig& config)
{
    config = parseBlasterString();
    return reset_dsp(config.base);
}
