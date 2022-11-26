#include <stdio.h>
#include <stdlib.h>

#include <vector>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <math.h>
#include <map>
#include <limits>
#include <list>

// #include <omp.h>

#include "command_line_parser.h"

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
    fread(&data[0], 1, fileSize, fp);
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

    fwrite(&data[0], data.size(), 1, fp);

    fclose(fp);
}

#define clamp(value, smallest, largest) \
    std::max((smallest), std::min((value), (largest)))


/**
 * This class is a decoder for 4bit Creative ADPCM
 * 
 * Sources:
 *  https://github.com/schlae/sb-firmware/blob/master/sbv202.asm
 *  https://github.com/joncampbell123/dosbox-x/blob/master/src/hardware/sblaster.cpp
 *  https://wiki.multimedia.cx/index.php/Creative_8_bits_ADPCM
 */
class CreativeAdpcmDecoder4Bit
{
public:
    CreativeAdpcmDecoder4Bit(uint8_t firstValue) :
        m_accumulator(1),                           // initialize accumulator to 1
        m_previous(firstValue)
    {}

    /**
     * Decode a nibble (4 bits) of data and return the 8bit data.
     * 
     * @param nibble The 4bits to be decoded. Value must be smaller than 16.
     */
    uint8_t decodeNibble(uint8_t nibble)
    {
        int sign = (nibble & 8)?-1:1;               // Input is just 4 bits (a nibble), so the 4th bit is the sign bit
        uint8_t data = nibble & 7;                  // The lower 3 bits are the sample data
        uint8_t delta = 
            (data * m_accumulator) +
            (m_accumulator / 2);                    // Scale sample data using accumulator value
        int result = m_previous + (sign * delta);   // Calculate the next value
        m_previous = clamp(result, 0, 255);         // Limit value to 0..255

        if ((data == 0) && (m_accumulator > 1))     // If input value is 0, and accumulator is
            m_accumulator /= 2;                     // larger than 1, then halve accumulator.
        if ((data >= 5) && (m_accumulator < 8))     // If input value larger than 5, and accumulator is
            m_accumulator *= 2;                     // lower than 8, then double accumulator.

        return m_previous;
    }

private:
    uint8_t m_accumulator;
    uint8_t m_previous;
};

uint64_t getNthNibble(int n, uint64_t value)
{
    return (0xf & (value >> (4 * n)));
}


constexpr uint64_t constPow(uint64_t val, uint64_t exp)
{
    uint64_t res = 1;
    while (exp-- > 0)
    {
        res *= val;
    }

    return res;
}


struct Best
{
    uint64_t bestIndex = 0;
    uint64_t bestDiff = std::numeric_limits<uint64_t>::max();
    CreativeAdpcmDecoder4Bit bestDecoder = CreativeAdpcmDecoder4Bit(0);
};

/**
 * Encodes the given sequence of unsigned 8bit values to 4bit ADPCM.
 * The first 8bit value is stored "as is", but the following values are
 * compressed to 4bit values. This almost halves the size.
 * 
 * The encoder uses an ADPCM decoder and tries every possible input
 * until it gets the output that most closely matches the input value.
 * The parameter combinedNibbles controlls the number of nibbles
 * that are combined. Each additional nibble muliplies the runtime
 * by 16. A value between 3 and 4 produces good results.
 * Increasing the number further does not add much quality improvements,
 * but drastically increases the runtime, so 4 is the default.
 */
#if 0
std::vector<uint8_t> createAdpcm4BitFromRawOpenMP(const std::vector<uint8_t>& raw, uint64_t combinedNibbles = 5)
{
    uint64_t squaredSum = 0u;

    CreativeAdpcmDecoder4Bit decoder(raw[0]);
    
    std::vector<uint64_t> result(raw.size() / combinedNibbles);

    for (int i = 1; i < raw.size() / combinedNibbles; ++i)
    {
        std::vector<Best> bestResults(omp_get_max_threads());

        // try every possible input for the decoder
        #pragma omp parallel for
        for (uint64_t n = 0; n < constPow(16, combinedNibbles); ++n)
        {
            CreativeAdpcmDecoder4Bit decoderCopy = decoder;
            uint64_t diffSum = 0;
            for (int nib = 0; nib < combinedNibbles; ++nib)
            {
                int diff = std::abs(decoderCopy.decodeNibble(getNthNibble(nib, n)) - raw[i*combinedNibbles + nib]);
                diffSum += diff * diff;
            }
 
            auto& best = bestResults.at(omp_get_thread_num());

            if (diffSum < best.bestDiff)
            {
                best.bestDiff = diffSum;
                best.bestIndex = n;
                best.bestDecoder = decoderCopy;
            }
        }

        // now merge results from threads
        uint64_t bestIndex = 0;
        uint64_t bestDiff = std::numeric_limits<uint64_t>::max();
        CreativeAdpcmDecoder4Bit bestDecoder(0);
        for (auto& best : bestResults)
        {
            // printf("diff: %d\n", best.bestDiff);
            if (best.bestDiff < bestDiff)
            {
                bestDiff = best.bestDiff;
                bestIndex = best.bestIndex;
                bestDecoder = best.bestDecoder;
            }
        }


        decoder = bestDecoder; 
        result[i-1] = bestIndex;
        squaredSum += bestDiff;


        if (i % 10 == 0) printf("%d\n", i);
    }

    printf("sum: %ld\n", squaredSum);

    std::vector<uint8_t> nibbles(result.size() * combinedNibbles);
    for (int i = 0; i < result.size(); ++i)
    {
        for (int nib = 0; nib < combinedNibbles; ++nib)
        {
            nibbles[i * combinedNibbles + nib] = getNthNibble(nib, result[i]);
        }
    }

    std::vector<uint8_t> binaryResult(nibbles.size() / 2);

    // merge nibbles into bytes
    for (int n = 0; n < nibbles.size() / 2; ++n)
    {
        binaryResult[n] = ((nibbles[2 * n] << 4) + (nibbles[2 * n + 1]));
    }

    binaryResult.insert(binaryResult.begin(), raw[0]);

    return binaryResult;
}
#endif

/**
 * Encodes the given sequence of unsigned 8bit values to 4bit ADPCM.
 * The first 8bit value is stored "as is", but the following values are
 * compressed to 4bit values. This almost halves the size.
 * 
 * The encoder uses an ADPCM decoder and tries every possible input
 * until it gets the output that most closely matches the input value.
 * The parameter combinedNibbles controlls the number of nibbles
 * that are combined. Each additional nibble muliplies the runtime
 * by 16. A value between 3 and 4 produces good results.
 * Increasing the number further does not add much quality improvements,
 * but drastically increases the runtime, so 4 is the default.
 */
std::vector<uint8_t> createAdpcm4BitFromRaw(const std::vector<uint8_t>& raw, uint64_t combinedNibbles = 4)
{
    uint64_t squaredSum = 0u;

    CreativeAdpcmDecoder4Bit decoder(raw[0]);
    
    std::vector<uint64_t> result(raw.size() / combinedNibbles);

    for (int i = 1; i < raw.size() / combinedNibbles; ++i)
    {
        Best bestResults;

        // try every possible input for the decoder
        for (uint64_t n = 0; n < constPow(16, combinedNibbles); ++n)
        {
            CreativeAdpcmDecoder4Bit decoderCopy = decoder;
            uint64_t diffSum = 0;
            for (int nib = 0; nib < combinedNibbles; ++nib)
            {
                int diff = decoderCopy.decodeNibble(getNthNibble(nib, n)) - raw[i*combinedNibbles + nib];
                diffSum += diff * diff;
            }
 
            if (diffSum < bestResults.bestDiff)
            {
                bestResults.bestDiff = diffSum;
                bestResults.bestIndex = n;
                bestResults.bestDecoder = decoderCopy;
            }
        }

        decoder = bestResults.bestDecoder; 
        result[i-1] = bestResults.bestIndex;
        squaredSum += bestResults.bestDiff;


        // if (i % 10 == 0) printf("%d\n", i);
    }

    // printf("sum: %ld\n", squaredSum);

    std::vector<uint8_t> nibbles(result.size() * combinedNibbles);
    for (int i = 0; i < result.size(); ++i)
    {
        for (int nib = 0; nib < combinedNibbles; ++nib)
        {
            nibbles[i * combinedNibbles + nib] = getNthNibble(nib, result[i]);
        }
    }

    std::vector<uint8_t> binaryResult(nibbles.size() / 2);

    // merge nibbles into bytes
    for (int n = 0; n < nibbles.size() / 2; ++n)
    {
        binaryResult[n] = ((nibbles[2 * n] << 4) + (nibbles[2 * n + 1]));
    }

    binaryResult.insert(binaryResult.begin(), raw[0]);

    return binaryResult;
}

struct Path
{
    uint64_t squaredSum = 0;
    std::vector<uint8_t> history;
    CreativeAdpcmDecoder4Bit decoder = CreativeAdpcmDecoder4Bit(0);

    bool operator<(const Path& other)
    {
        return this->squaredSum < other.squaredSum;
    }
};

void fillEmptyList(std::list<Path>& emptyList, size_t historySize, size_t elementCount)
{
    for (int i = 0; i < elementCount; ++i)
    {
        emptyList.push_back({});
        emptyList.back().history.reserve(historySize);
    }
}


std::vector<uint8_t> createAdpcm4BitFromRawExperiment(const std::vector<uint8_t>& raw)
{
    std::list<Path> leafs;
    uint32_t maxSize = 16 * 16 * 16 * 2;

    std::list<Path> emptyList;
    // for (int i = 0; i < maxSize; ++i)
    // {
    //     emptyList.push_back({});
    //     emptyList.back().history.reserve(raw.size() + 1);
    // }

    fillEmptyList(emptyList, raw.size(), maxSize * 16);

    leafs.push_back({0, {}, CreativeAdpcmDecoder4Bit(raw[0])});

    for (int n = 1; n < raw.size(); ++n)
    {
        std::list<Path> newLeafs;

        for (auto& leaf : leafs)
        {
            for (int i = 0; i < 16; ++i)
            {
                if (emptyList.empty())
                {
                    fillEmptyList(emptyList, raw.size(), 1);
                }

                Path& p = emptyList.front();
                p.history = leaf.history;
                p.decoder = leaf.decoder;
                p.squaredSum = leaf.squaredSum;
                int diff = p.decoder.decodeNibble(i) - raw[n];
                p.squaredSum += diff * diff;
                p.history.push_back(i);
                // newLeafs.push_back(p);
                newLeafs.splice(newLeafs.end(), emptyList, emptyList.begin());
            }
        }

        // std::sort(newLeafs.begin(), newLeafs.end());
        newLeafs.sort();

        // remove worst paths, keep only maxSize paths
        if (newLeafs.size() > maxSize)
        {
            auto it = newLeafs.begin();
            std::advance(it, maxSize);
            emptyList.splice(emptyList.begin(), newLeafs, it, newLeafs.end()); // move unneeded elements to emptyList
            // newLeafs.erase(it, newLeafs.end());
        }

        // remove old leaves to emptyList
        emptyList.splice(emptyList.begin(), leafs, leafs.begin(), leafs.end());

        leafs.splice(leafs.begin(), newLeafs, newLeafs.begin(), newLeafs.end());

        // leafs = std::move(newLeafs);

        if (n % 10 == 0){
            printf("%d -> %ld (%ld)\n", n, raw.size(), leafs.size());
            printf("newLeafs: %ld\n", newLeafs.size());
        }
    }

    // std::sort(leafs.begin(), leafs.end());
    leafs.sort();

    printf("res: %ld\n", leafs.front().squaredSum);


    auto& nibbles = leafs.front().history;

    std::vector<uint8_t> binaryResult(nibbles.size() / 2);

    // merge nibbles into bytes
    for (int n = 0; n < nibbles.size() / 2; ++n)
    {
        binaryResult[n] = ((nibbles[2 * n] << 4) + (nibbles[2 * n + 1]));
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


int intRound(double x)
{
    if (x < 0.0)
        return (int)(x - 0.5);
    else
        return (int)(x + 0.5);
}

std::vector<uint8_t> createVocFile(
    uint32_t frequency,
    const std::vector<uint8_t>& sampleData,
    VocSampleFormat sampleFormat)
{
    uint8_t timeConstant = intRound(256 - 1000000.0 / frequency);

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


std::map <std::string, VocSampleFormat> compressionFormats =
{
    {"PCM", VOC_FORMAT_PCM_8BIT},
    {"ADPCM4", VOC_FORMAT_ADPCM_4BIT},
};

int main(int argc, char* argv[])
{
    try
    {
        clp::CommandLineParser parser(
            "Program to convert raw sound files into VOC files including ADPCM compression.\n"
            "Compression formats:\n"
            "  PCM    - unsigned integer 8-bit per sample\n"
            "  ADPCM4 - ADPCM 4-bit per sample\n");
        parser.addParameter("input", "i", "Name of the input file", clp::ParameterRequired::yes);
        parser.addParameter("frequency", "f", "Frequency of input file in hertz", clp::ParameterRequired::yes);
        parser.addParameter("output", "o", "Name of the output file", clp::ParameterRequired::yes);
        parser.addParameter("compression", "c", "Compression to be used. Options: PCM, ADPCM4", clp::ParameterRequired::no, "ADPCM4");

        parser.parse(argc, argv);

        VocSampleFormat format;
        try {
            format = compressionFormats.at(parser.getValue<std::string>("compression"));
        }
        catch (...)
        {
            printf("invalid compression format\n");
            return 1;
        }

        std::vector<uint8_t> sampleData;

        switch(format)
        {
            case VOC_FORMAT_ADPCM_4BIT:
            {
                std::vector<uint8_t> raw = loadFile(parser.getValue<std::string>("input"));
                sampleData = createAdpcm4BitFromRawExperiment(raw);
                break;
            }
            case VOC_FORMAT_PCM_8BIT:
            {
                sampleData = loadFile(parser.getValue<std::string>("input"));
                break;
            }
        }

        std::vector<uint8_t> vocData = createVocFile(parser.getValue<uint32_t>("frequency"), sampleData, format);

        storeFile(parser.getValue<std::string>("output"), vocData);

        return 0;
    }
    catch (const std::exception &e)
    {
        printf("Error: %s\n", e.what());
        return 1;
    }
}
