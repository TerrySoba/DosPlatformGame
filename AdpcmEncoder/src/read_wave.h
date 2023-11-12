#ifndef READ_WAVE_H
#define READ_WAVE_H

#include <vector>
#include <cstdint>
#include <array>

struct WaveFileHeader
{
    std::array<char, 4> chunkId;
    uint32_t chunkSize;
    std::array<char, 4> format;
    std::array<char, 4> subChunk1Id;
    uint32_t subChunk1Size;
    uint16_t audioFormat;
    uint16_t numChannels;
    uint32_t sampleRate;
    uint32_t byteRate;
    uint16_t bytesPerSample;
    uint16_t bitsPerSample;
};

struct WaveFile
{
    WaveFileHeader header;
    std::vector<uint8_t> rawData;
};

WaveFile loadWaveFile(const char* filename);

struct WaveFileMono
{
    uint32_t sampleRate;
    std::vector<float> data;
};

/**
 * @brief Loads a wave file and converts it to mono. The samples are converted to float.
 */
WaveFileMono loadWaveFileToMono(const char *filename);


#endif
