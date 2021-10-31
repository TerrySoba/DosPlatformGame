#include "soundblaster.h"

#include <dos.h>
#include <conio.h>
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "exception.h"

#define SB_RESET 0x6
#define SB_READ_DATA 0xA
#define SB_READ_DATA_STATUS 0xE
#define SB_WRITE_DATA 0xC

#define SB_GET_DSP_VERSION_NUMBER 0xE1

#define SB_ENABLE_SPEAKER 0xD1
#define SB_DISABLE_SPEAKER 0xD3
#define SB_SET_PLAYBACK_FREQUENCY 0x40
#define SB_SINGLE_CYCLE_PLAYBACK 0x14

#define MASK_REGISTER 0x0A
#define MODE_REGISTER 0x0B
#define MSB_LSB_FLIP_FLOP 0x0C
#define DMA_CHANNEL_0 0x87
#define DMA_CHANNEL_1 0x83
#define DMA_CHANNEL_3 0x82




static uint16_t SoundBlaster::s_base; /* default 220h */
static uint8_t SoundBlaster::s_irq; /* default 7 */
static uint8_t SoundBlaster::s_dma; /* default 1 */
static SoundblasterType SoundBlaster::s_type;
static volatile bool SoundBlaster::s_playing;


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

struct SbConfig {
    uint16_t base; /* default 220h */
    uint8_t irq; /* default 7 */
    uint8_t dma; /* default 1 */
    SoundblasterType type;
};

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

void SoundBlaster::writeDsp(uint8_t command)
{
    while ((inp(s_base + SB_WRITE_DATA) & 0x80) == 0x80);
    outp(s_base + SB_WRITE_DATA, command);
}

uint8_t SoundBlaster::readDsp()
{
    while ((inp(s_base + SB_READ_DATA_STATUS) & 0x80) != 0x80);
    return inp(s_base + SB_READ_DATA);
}

bool isHighIrq(uint8_t irq)
{
    return (irq == 2 || irq == 10 || irq == 11);
}

uint8_t highIrqToVector(uint8_t irq)
{
    switch(irq)
    {
        case 2:
            return 0x71;
        case 10:
            return 0x72;
        case 11:
            return 0x73;
        default:
            return 0;
    }
}

void __interrupt SoundBlaster::sbIrqHandler()
{
    __asm { cli }
    inp(s_base + SB_READ_DATA_STATUS);
    outp(0x20, 0x20); // satisfy PIC
    if (isHighIrq(s_irq)) {
        outp(0xA0, 0x20);
    }

    s_playing = 0;
    __asm { sti }
}

void SoundBlaster::initIrq()
{
    // save and replace IRQ vector
    if (isHighIrq(s_irq)) {
        m_oldIrq = _dos_getvect(highIrqToVector(s_irq));
        _dos_setvect(highIrqToVector(s_irq), sbIrqHandler);

        // enable IRQ with the mainboard's PIC
        if( s_irq == 2) outp( 0xA1, inp( 0xA1 ) & 253 );
        if( s_irq == 10) outp( 0xA1, inp( 0xA1 ) & 251 );
        if( s_irq == 11) outp( 0xA1, inp( 0xA1 ) & 247 );
        outp( 0x21, inp( 0x21 ) & 251 );
    } else {
        m_oldIrq = _dos_getvect(s_irq + 8);
        _dos_setvect(s_irq + 8, sbIrqHandler);
        
        // enable IRQ with the mainboard's PIC
        outp(0x21, inp(0x21) & !(1 << s_irq)); 
    }
}

void SoundBlaster::deinitIrq()
{
    // save and replace IRQ vector
    if (isHighIrq(s_irq)) {
        _dos_setvect(highIrqToVector(s_irq), m_oldIrq);

        // enable IRQ with the mainboard's PIC
        if(s_irq == 2) outp( 0xA1, inp( 0xA1 ) | 2 );
        if(s_irq == 10) outp( 0xA1, inp( 0xA1 ) | 4 );
        if(s_irq == 11) outp( 0xA1, inp( 0xA1 ) | 8 );
        outp(0x21, inp(0x21) | 4 );
    } else {
        _dos_setvect(s_irq + 8, m_oldIrq);
        outp(0x21, inp(0x21) & (1 << s_irq)); 
    }
}

void SoundBlaster::assignDmaBuffer()
{
    uint8_t* tmpBuf;
    uint32_t linearAddress;
    uint16_t page1, page2;
    
    tmpBuf = (uint8_t*)_fmalloc(32768);
    linearAddress = FP_SEG(tmpBuf);
    linearAddress = (linearAddress << 4) + FP_OFF(tmpBuf);
    page1 = linearAddress >> 16;
    page2 = (linearAddress + 32767) >> 16;

    m_dmaBuffer = tmpBuf;
    m_dmaPage = linearAddress >> 16;
    m_dmaOffset = linearAddress & 0xFFFF;
}


void SoundBlaster::singleCyclePlayback()
{
    s_playing = true;

    // program the DMA controller
    outp(MASK_REGISTER, 4 | s_dma);
    outp(MSB_LSB_FLIP_FLOP, 0);
    outp(MODE_REGISTER, 0x48 | s_dma);
    outp(s_dma << 1, m_dmaOffset & 0xFF);
    outp(s_dma << 1, m_dmaOffset >> 8);
    switch (s_dma)
    {
    case 0:
        outp(DMA_CHANNEL_0, m_dmaPage);
        break;
    case 1:
        outp(DMA_CHANNEL_1, m_dmaPage);
        break;
    case 3:
        outp(DMA_CHANNEL_3, m_dmaPage);
        break;
    }
    outp((s_dma << 1) + 1, m_toBePlayed & 0xFF);
    outp((s_dma << 1) + 1, m_toBePlayed >> 8);
    outp(MASK_REGISTER, s_dma);
    writeDsp(SB_SINGLE_CYCLE_PLAYBACK);
    writeDsp(m_toBePlayed & 0xFF);
    writeDsp(m_toBePlayed >> 8);
    m_toBePlayed = 0;
}


void SoundBlaster::singlePlay(const char* fileName)
{
    FILE *rawFile;
    int fileSize;

    memset(m_dmaBuffer, 0, 32768);
    rawFile = fopen(fileName, "rb");
    fseek(rawFile, 0L, SEEK_END);
    fileSize = ftell(rawFile);
    fseek(rawFile, 0L, SEEK_SET);
    fread(m_dmaBuffer, 1, fileSize, rawFile);
    writeDsp(SB_SET_PLAYBACK_FREQUENCY);
    writeDsp(256 - 1000000 / 11000);
    m_toBePlayed = fileSize - 1;

    singleCyclePlayback();
    while (s_playing);
}

void SoundBlaster::singlePlay(const SbSample& sample)
{
    writeDsp(SB_SET_PLAYBACK_FREQUENCY);
    writeDsp(sample.timeConstant);

    uint32_t linearAddress = FP_SEG(sample.data);
    linearAddress = (linearAddress << 4) + FP_OFF(sample.data);

    uint16_t dmaPage = linearAddress >> 16;
    uint16_t dmaOffset = linearAddress & 0xFFFF;
    uint16_t toBePlayed = sample.length - 1;

    s_playing = true;

    // program the DMA controller
    outp(MASK_REGISTER, 4 | s_dma);
    outp(MSB_LSB_FLIP_FLOP, 0);
    outp(MODE_REGISTER, 0x48 | s_dma);
    outp(s_dma << 1, dmaOffset & 0xFF);
    outp(s_dma << 1, dmaOffset >> 8);
    switch (s_dma)
    {
    case 0:
        outp(DMA_CHANNEL_0, dmaPage);
        break;
    case 1:
        outp(DMA_CHANNEL_1, dmaPage);
        break;
    case 3:
        outp(DMA_CHANNEL_3, dmaPage);
        break;
    }
    outp((s_dma << 1) + 1, toBePlayed & 0xFF);
    outp((s_dma << 1) + 1, toBePlayed >> 8);
    outp(MASK_REGISTER, s_dma);
    writeDsp(SB_SINGLE_CYCLE_PLAYBACK);
    writeDsp(toBePlayed & 0xFF);
    writeDsp(toBePlayed >> 8);


    // while (s_playing);
}

SbVersion SoundBlaster::getDspVersion()
{
    writeDsp(SB_GET_DSP_VERSION_NUMBER);
    // while(inp(s_base + SB_READ_DATA_STATUS) & 0x80 != 0x80);
    SbVersion version;
    version.major = readDsp();// inp(s_base + SB_READ_DATA);
    version.minor = readDsp();// inp(s_base + SB_READ_DATA);
    return version;
}


SoundBlaster::SoundBlaster()
{
    s_playing = false;

    SbConfig config = parseBlasterString();
    s_base = config.base;
    s_irq = config.irq;
    s_dma = config.dma;
    s_type = config.type;

    // printf("Found Soundblaster: base:0x%x irq:%d dma:%d type:%d\n", config.base, config.irq, config.dma, config.type);

    m_sbFound = reset_dsp(s_base);

    if (m_sbFound) {
        initIrq();
        // assignDmaBuffer();
        writeDsp(SB_ENABLE_SPEAKER);
    }
}


SoundBlaster::~SoundBlaster()
{
    if (m_sbFound) {
        writeDsp(SB_DISABLE_SPEAKER);
        free(m_dmaBuffer);
        deinitIrq();
    }
}

SbSample SoundBlaster::loadRawSample(const char* filename, uint16_t sampleRate /* in Hz */)
{
    FILE *rawFile;
    int fileSize;
    rawFile = fopen(filename, "rb");
    if (!rawFile)
    {
        throw Exception("Could not open file: ", filename);
    }
    fseek(rawFile, 0L, SEEK_END);
    fileSize = ftell(rawFile);
    fseek(rawFile, 0L, SEEK_SET);
    SbSample sample;
    sample.length = fileSize;
    sample.data = (uint8_t*)malloc(sample.length);
    fread(sample.data, 1, sample.length, rawFile);
    sample.timeConstant = 256 - 1000000 / 11000;

    return sample;
}
