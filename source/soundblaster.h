#ifndef _SOUNDBLASTER_H_INC
#define _SOUNDBLASTER_H_INC

#include <stdint.h>

enum SoundblasterType {
    SOUNDBLASTER_UNKNOWN = 0,
    SOUNDBLASTER_1 = 1,
    SOUNDBLASTER_PRO = 2,
    SOUNDBLASTER_2 = 3,
    SOUNDBLASTER_PRO2 = 4,
    SOUNDBLASTER_PRO_MCA = 5,
    SOUNDBLASTER_16 = 6,
};

struct SbVersion
{
    uint8_t major;
    uint8_t minor;
};

enum PackMethod
{
    PCM_8BIT = 0,
    ADPCM_4BIT = 1,
    ADPCM_3BIT = 2,
    ADPCM_2BIT = 3,
};

struct SbSample
{
    uint16_t length;
    uint8_t* data;
    uint8_t timeConstant; // Transfer time constant = 256 - 1000000 / frequency
    PackMethod packMethod;
};


/**
 * Code adapted from https://gist.github.com/root42/7e2be31d755d89eb0cf0350ad066c53c
 * 
 * Thank you root42!
 */
class SoundBlaster
{
public:
    SoundBlaster();
    ~SoundBlaster();

    bool soundBlasterFound() { return m_sbFound; }

    uint16_t getBasePort() { return s_base; }
    uint8_t getIRQ() { return s_irq; }
    uint8_t getDMA() { return s_dma; }
    uint8_t getType() { return s_type; }

    bool isPlaying() { return s_playing; }

    void singlePlay(const char* fileName);
    void singlePlay(const SbSample& sample);

    static SbSample loadRawSample(const char* filename, uint16_t sampleRate /* in Hz */);
    static SbSample loadVocFile(const char* filename);


    SbVersion getDspVersion();


private: // methods
    void writeDsp(uint8_t command);
    uint8_t readDsp();
    void initIrq();
    void deinitIrq();
    void assignDmaBuffer();
    void singleCyclePlayback();

    static void __interrupt sbIrqHandler();

private: // member variables
    static uint16_t s_base; /* default 220h */
    static uint8_t s_irq; /* default 7 */
    static uint8_t s_dma; /* default 1 */
    static SoundblasterType s_type;
    static volatile bool s_playing;

    bool m_sbFound;

    void __interrupt (*m_oldIrq)();

    uint8_t* m_dmaBuffer;
    uint16_t m_dmaPage;
    uint16_t m_dmaOffset;
    uint16_t m_toBePlayed;
};

#endif
