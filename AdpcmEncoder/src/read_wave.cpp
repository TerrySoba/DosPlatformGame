#include "read_wave.h"

#include <stdexcept>
#include <cstring>
#include <iostream>
#include <memory>
#include "resampling.h"

void safeRead(void* buffer, size_t size, size_t count, FILE* file)
{
    if (fread(buffer, size, count, file) != count)
    {
        throw std::runtime_error("Failed to read file");
    }
}


WaveFileHeader readWaveFileHeder(FILE* file)
{
    WaveFileHeader header;
    safeRead(header.chunkId.data(), 4, 1, file);
    if (memcmp(header.chunkId.data(), "RIFF", 4) != 0)
    {
        throw std::runtime_error("Invalid file format, expected RIFF");
    }

    safeRead(&header.chunkSize, 4, 1, file);
    safeRead(header.format.data(), 4, 1, file);
    if (memcmp(header.format.data(), "WAVE", 4) != 0)
    {
        throw std::runtime_error("Invalid file format, expected WAVE");
    }

    safeRead(header.subChunk1Id.data(), 4, 1, file);
    if (memcmp(header.subChunk1Id.data(), "fmt ", 4) != 0)
    {
        throw std::runtime_error("Invalid file format, expected fmt");
    }

    safeRead(&header.subChunk1Size, 4, 1, file);
    safeRead(&header.audioFormat, 2, 1, file);
    safeRead(&header.numChannels, 2, 1, file);
    safeRead(&header.sampleRate, 4, 1, file);
    safeRead(&header.byteRate, 4, 1, file);
    safeRead(&header.bytesPerSample, 2, 1, file);
    safeRead(&header.bitsPerSample, 2, 1, file);

    return header;
}


std::vector<uint8_t> readWaveDataChunk(FILE* file)
{
    std::array<char, 4> subChunk2Id;
    uint32_t subChunk2Size;
    safeRead(subChunk2Id.data(), 4, 1, file);
    if (memcmp(subChunk2Id.data(), "data", 4) != 0)
    {
        throw std::runtime_error("Invalid file format, expected data");
    }

    safeRead(&subChunk2Size, 4, 1, file);

    std::vector<uint8_t> data(subChunk2Size);
    safeRead(data.data(), subChunk2Size, 1, file);

    return data;
}


WaveFile loadWaveFile(const char* filename)
{
    auto file = std::shared_ptr<FILE>(
        fopen(filename, "rb"),
        [](FILE* file) { if (file) {fclose(file);} });
    if (!file)
    {
        throw std::runtime_error("Failed to open file");
    }

    WaveFileHeader header = readWaveFileHeder(file.get());

    auto data = readWaveDataChunk(file.get());

    WaveFile waveFile;
    waveFile.header = header;
    waveFile.rawData = std::move(data);
    return waveFile;
}


WaveFileMono loadWaveFileToMono(const char *filename)
{
    auto waveFile = loadWaveFile(filename);
    std::vector<float> output;
    if (waveFile.header.bitsPerSample == 16)
    {
        std::vector<int16_t> samples;
        samples.reserve(waveFile.rawData.size() / 2);
        for (size_t i = 0; i < waveFile.rawData.size(); i += 2)
        {
            int16_t sample = waveFile.rawData[i] | (waveFile.rawData[i + 1] << 8);
            samples.push_back(sample);
        }
        output = toFloatVector(samples);
    }
    else if (waveFile.header.bitsPerSample == 8)
    {
        output = toFloatVector(waveFile.rawData);
    }
    else
    {
        throw std::runtime_error("Only 8 and 16 bit files supported");
    }

    if (waveFile.header.numChannels > 1)
    {
        // merge float samples to mono
        for (size_t i = 0; i < output.size(); i += waveFile.header.numChannels)
        {
            float sample = 0;
            for (size_t j = 0; j < waveFile.header.numChannels; ++j)
            {
                sample += output[i + j];
            }
            sample /= waveFile.header.numChannels;
            output[i / waveFile.header.numChannels] = sample;
        }

        // remove unused samples
        output.resize(output.size() / waveFile.header.numChannels);
    }

    WaveFileMono waveFileMono;
    waveFileMono.sampleRate = waveFile.header.sampleRate;
    waveFileMono.data = std::move(output);
    return waveFileMono;
}
