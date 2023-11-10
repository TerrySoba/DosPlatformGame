#ifndef VOC_FORMAT_H
#define VOC_FORMAT_H

#include <vector>
#include <cstdint>

enum VocSampleFormat
{
    VOC_FORMAT_PCM_8BIT = 0,
    VOC_FORMAT_ADPCM_4BIT = 1,
    VOC_FORMAT_ADPCM_3BIT = 2,
    VOC_FORMAT_ADPCM_2BIT = 3,
};

std::vector<uint8_t> createVocFile(uint32_t frequency,
                                   const std::vector<uint8_t> &sampleData,
                                   VocSampleFormat sampleFormat);

#endif