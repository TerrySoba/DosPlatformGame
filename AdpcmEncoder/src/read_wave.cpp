#include "read_wave.h"

#include <stdexcept>
#include <cstring>
#include <iostream>
#include <memory>
#include <sstream>
#include "resampling.h"

void safeRead(void* buffer, size_t size, size_t count, FILE* file)
{
    if (fread(buffer, size, count, file) != count)
    {
        throw std::runtime_error("Failed to read file");
    }
}


WaveFileHeader readWaveFileHeader(FILE* file)
{
    WaveFileHeader header;
    safeRead(header.chunkId.data(), 4, 1, file);
    if (memcmp(header.chunkId.data(), "RIFF", 4) != 0)
    {
        throw std::runtime_error("Invalid file format, expected RIFF chunk.");
    }

    safeRead(&header.chunkSize, 4, 1, file);
    safeRead(header.format.data(), 4, 1, file);
    if (memcmp(header.format.data(), "WAVE", 4) != 0)
    {
        throw std::runtime_error("Invalid file format, expected WAVE chunk.");
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
    // safeRead(subChunk2Id.data(), 4, 1, file);
    // if (memcmp(subChunk2Id.data(), "data", 4) != 0)
    // {
    //     throw std::runtime_error("Invalid file format, expected data chunk.");
    // }

    safeRead(&subChunk2Size, 4, 1, file);

    std::vector<uint8_t> data(subChunk2Size);
    safeRead(data.data(), subChunk2Size, 1, file);

    return data;
}


std::string readChunkId(FILE* file)
{
    std::array<char, 4> chunkId;
    safeRead(chunkId.data(), 4, 1, file);
    return std::string(chunkId.data(), chunkId.size());
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

    WaveFileHeader header = readWaveFileHeader(file.get());

    std::vector<uint8_t> data;

    while (!feof(file.get()))
    {
        std::string chunkId;
        try
        {
            chunkId = readChunkId(file.get());
        }
        catch (...)
        {
            break;
        }

        if (chunkId == "data")
        {
            data = readWaveDataChunk(file.get());
        }
        else
        {
            uint32_t chunkSize;
            safeRead(&chunkSize, 4, 1, file.get());
            fseek(file.get(), chunkSize, SEEK_CUR);
        }
    }


    WaveFile waveFile;
    waveFile.header = header;
    waveFile.rawData = std::move(data);
    return waveFile;
}

enum WaveAudioFormat
{
    WAVE_FORMAT_PCM = 1,
    WAVE_FORMAT_IEEE_FLOAT = 3
};


WaveFileMono loadWaveFileToMono(const char *filename)
{
    auto waveFile = loadWaveFile(filename);
    std::vector<float> output;

    if (waveFile.header.audioFormat == WAVE_FORMAT_PCM)
    {
        switch(waveFile.header.bitsPerSample)
        {
            case 32:
            {
                std::vector<int32_t> samples;
                samples.reserve(waveFile.rawData.size() / 4);
                for (size_t i = 0; i < waveFile.rawData.size(); i += 4)
                {
                    int32_t sample = (waveFile.rawData[i + 0] << 0) | (waveFile.rawData[i + 1] << 8) | (waveFile.rawData[i + 2] << 16) | (waveFile.rawData[i + 3] << 24);
                    samples.push_back(sample);
                }
                output = toFloatVector(samples);
                break;
            }

            case 24:
            {
                std::vector<int32_t> samples;
                samples.reserve(waveFile.rawData.size() / 3);
                for (size_t i = 0; i < waveFile.rawData.size(); i += 3)
                {
                    int32_t sample = (waveFile.rawData[i] << 8) | (waveFile.rawData[i + 1] << 16) | (waveFile.rawData[i + 2] << 24);
                    samples.push_back(sample);
                }
                output = toFloatVector(samples);
                break;
            }   

            case 16:
            {
                std::vector<int16_t> samples;
                samples.reserve(waveFile.rawData.size() / 2);
                for (size_t i = 0; i < waveFile.rawData.size(); i += 2)
                {
                    int16_t sample = waveFile.rawData[i] | (waveFile.rawData[i + 1] << 8);
                    samples.push_back(sample);
                }
                output = toFloatVector(samples);
                break;
            }

            case 8:
            {
                output = toFloatVector(waveFile.rawData);
                break;
            }

            default:
            {   
                std::stringstream ss;
                ss << "Unsupported bits per sample: " << waveFile.header.bitsPerSample;
                throw std::runtime_error(ss.str());
            }
        }
    }
    else if (waveFile.header.audioFormat == WAVE_FORMAT_IEEE_FLOAT)
    {
        switch(waveFile.header.bitsPerSample)
        {
            case 32:
            {
                std::vector<float> samples;
                samples.reserve(waveFile.rawData.size() / 4);
                for (size_t i = 0; i < waveFile.rawData.size(); i += 4)
                {
                    float sample;
                    memcpy(&sample, &waveFile.rawData[i], 4);
                    samples.push_back(sample);
                }
                output = std::move(samples);
                break;
            }
            case 64:
            {
                std::vector<float> samples;
                samples.reserve(waveFile.rawData.size() / 8);
                for (size_t i = 0; i < waveFile.rawData.size(); i += 8)
                {
                    double sample;
                    memcpy(&sample, &waveFile.rawData[i], 8);
                    samples.push_back((float)sample);
                }
                output = std::move(samples);
                break;
            }
            default:
            {
                std::stringstream ss;
                ss << "Unsupported bits per sample float: " << waveFile.header.bitsPerSample;
                throw std::runtime_error(ss.str());
            }
        }
    }
    else
    {
        std::stringstream ss;
        ss << "Unsupported audio format: " << waveFile.header.audioFormat;
        throw std::runtime_error(ss.str());
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
