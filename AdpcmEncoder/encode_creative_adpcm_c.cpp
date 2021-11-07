#include <stdio.h>

#include <vector>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <math.h>

std::vector<uint8_t> loadFile(const std::string& filename)
{
    FILE* fp = fopen(filename.c_str(), "rb");
    if (!fp)
    {
        throw std::runtime_error("Could not open file: " +  filename);
    }
    fseek(fp, 0L, SEEK_END);
    size_t fileSize = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    std::vector<uint8_t> data(fileSize);
    fread(data.data(), 1, fileSize, fp);
    fclose(fp);
    return data;
}

void storeFile(const std::string& filename, const std::vector<uint8_t>& data)
{
    FILE* fp = fopen(filename.c_str(), "wb");
    if (!fp)
    {
        throw std::runtime_error("Could not open file: " +  filename);
    }

    fwrite(data.data(), data.size(), 1, fp);

    fclose(fp);
}


#define clamp(n, smallest, largest) std::max(smallest, std::min(n, largest))

std::vector<std::vector<uint8_t>> scaleMap = {
    {0, 1, 2, 3, 4, 5, 6, 7},
    {1, 3, 5, 7, 9, 11, 13, 15},
    {2, 6, 10, 14, 18, 22, 26, 30},
    {4, 12, 20, 28, 36, 44, 52, 60}};

std::vector<std::vector<int8_t>> adjustMap = {
    {0, 0, 0, 0, 0, 1, 1, 1},
    {-1, 0, 0, 0, 0, 1, 1, 1},
    {-1, 0, 0, 0, 0, 1, 1, 1},
    {-1, 0, 0, 0, 0, 0, 0, 0}};

class CreativeAdpcmDecoder4Bit
{
public:
    CreativeAdpcmDecoder4Bit(uint8_t firstValue) :
        m_reference(firstValue),
        m_scale(0)
    {}

    uint8_t decodeNibble(uint8_t nibble)
    {
        uint8_t negativeBit = nibble & (1 << 3);
        uint8_t sample = nibble & ((1 << 3) - 1);

        int sign = negativeBit ? -1 : 1;
        int ref = m_reference + (sign * scaleMap[m_scale][sample]);
        m_reference = clamp(ref, 0, 255);
        m_scale = m_scale + adjustMap[m_scale][sample];

        return m_reference;
    }

private:
    uint8_t m_reference;
    uint8_t m_scale;
};

std::vector<uint8_t> createAdpcm4BitFromRaw(const std::vector<uint8_t>& raw)
{
    CreativeAdpcmDecoder4Bit decoder(raw[0]);
    std::vector<uint8_t> result(raw.size() - 1);

    for (int i = 1; i < raw.size(); ++i)
    {
        int bestIndex = 0;
        int bestDiff = std::numeric_limits<int>::max();

        CreativeAdpcmDecoder4Bit bestDecoder(0);

        // try every possible input for the decoder
        for (uint8_t n = 0; n < 16; ++n)
        {
            CreativeAdpcmDecoder4Bit decoderCopy = decoder;
            int diff = std::abs(decoderCopy.decodeNibble(n) - raw[i]);
 
            if (diff < bestDiff)
            {
                bestDiff = diff;
                bestIndex = n;
                bestDecoder = decoderCopy;
            }
        }
        decoder = bestDecoder;
        result[i-1] = bestIndex;
    }

    std::vector<uint8_t> binaryResult(result.size() / 2);

    // merge nibbles into bytes
    for (int n = 0; n < result.size() / 2; ++n)
    {
        binaryResult[n] = ((result[2 * n] << 4) + (result[2 * n + 1]));
    }

    binaryResult.insert(binaryResult.begin(), raw[0]);

    return binaryResult;
}



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

enum VocSampleFormat
{
    VOC_FORMAT_PCM_8BIT = 0,
    VOC_FORMAT_ADPCM_4BIT = 1,
    VOC_FORMAT_ADPCM_3BIT = 2,
    VOC_FORMAT_ADPCM_2BIT = 3,
};

std::vector<uint8_t> createVocFile(
    uint32_t frequency,
    const std::vector<uint8_t>& sampleData,
    VocSampleFormat sampleFormat)
{
    uint8_t timeConstant = std::round(256 - 1000000.0 / frequency);

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

int main(int argc, char* argv[])
{
    try
    {
        if (argc != 4)
        {
            printf("Usage %s: <input> <frequency> <output>\n", argv[0]);
            exit(1);
        }

        auto raw = loadFile(argv[1]);
        auto compressed = createAdpcm4BitFromRaw(raw);
        auto vocData = createVocFile(atoi(argv[2]), compressed, VOC_FORMAT_ADPCM_4BIT);

        storeFile(argv[3], vocData);

        return 0;
    }
    catch (const std::exception &e)
    {
        printf("Error: %s\n", e.what());
        return 1;
    }
}
