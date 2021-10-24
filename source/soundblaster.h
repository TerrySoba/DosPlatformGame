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

struct SbConfig {
    uint16_t base; /* default 220h */
    uint8_t irq; /* default 7 */
    uint8_t dma; /* default 1 */
    SoundblasterType type;
};

bool sb_detect(SbConfig& config);
bool reset_dsp(uint16_t port);

#endif
