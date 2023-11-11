#include "voc_format.h"

#include <string>
#include <cmath>

void append(std::vector<uint8_t>& container, const std::string& value)
{
    container.insert(container.end(), value.c_str(), value.c_str() + value.size());
}

void append(std::vector<uint8_t>& container, uint16_t value)
{
    container.push_back(value & 0xff);
    container.push_back((value >> 8) & 0xff);
}

void append(std::vector<uint8_t>& container, uint8_t value)
{
    container.push_back(value);
}

std::vector<uint8_t> createVocFile(
    uint32_t frequency,
    const std::vector<uint8_t>& sampleData,
    VocSampleFormat sampleFormat)
{
    uint8_t timeConstant = round(256 - 1000000.0 / frequency);

    std::string vocHeader = "Creative Voice File\x1a";

    uint16_t major = 1;
    uint16_t minor = 10;
    uint16_t version = minor + (major << 8);
    uint16_t versionCheck = (~version + 0x1234);

    std::vector<uint8_t> out;

    append(out, vocHeader);
    append(out, (uint16_t)0x1a);
    append(out, version);
    append(out, versionCheck);
    append(out, (uint8_t)1); // sample header

    // now append size (3 bytes)
    uint32_t sampleSize = sampleData.size() + 2;
    out.push_back(sampleSize & 0xff);
    out.push_back(sampleSize >> 8 & 0xff);
    out.push_back(sampleSize >> 16 & 0xff);

    append(out, timeConstant);
    append(out, (uint8_t)sampleFormat);

    out.insert(out.end(), sampleData.begin(), sampleData.end());

    append(out, (uint8_t)0); // end marker

    return out;
}
