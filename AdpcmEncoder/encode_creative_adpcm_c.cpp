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
#include <cassert>
#include <array>

#include <omp.h>

#include "vectorclass/vectorclass.h"

#include "command_line_parser.h"

uint64_t nibbleDecodings = 0;

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
    [[maybe_unused]] auto bytes = fread(&data[0], 1, fileSize, fp);
    assert(fileSize == bytes);
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

void calculateAllNibbles(Vec16uc& previous, Vec16uc& accumulators)
{
    Vec16uc nibbles(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
    Vec16uc data = nibbles & 7;

    Vec16uc delta =
        (data * accumulators) +
        (accumulators / 2);

    previous = select((nibbles & 8) != 0, add_saturated(previous, delta), sub_saturated(previous, delta));

    Vec16cb mask = (data == 0) & (accumulators > 1);
    accumulators = select(mask, accumulators >> 1, accumulators);

    mask = (data >= 5) & (accumulators < 8);
    accumulators = select(mask, accumulators << 1, accumulators);
}


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
        nibbleDecodings++;
        int sign = (nibble & 8) / 4 - 1;            // Input is just 4 bits (a nibble), so the 4th bit is the sign bit
        uint8_t data = nibble & 7;                  // The lower 3 bits are the sample data
        uint8_t delta = 
            (data * m_accumulator) +
            (m_accumulator / 2);                    // Scale sample data using accumulator value
        int result = m_previous + (sign * delta);   // Calculate the next value
        m_previous = std::clamp(result, 0, 255);    // Limit value to 0..255

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


std::vector<uint8_t> decodeAdpcm4(uint8_t initial, std::vector<uint8_t> nibbles)
{
    CreativeAdpcmDecoder4Bit decoder(initial);

    std::vector<uint8_t> decoded(nibbles.size() + 1);
    decoded[0] = initial;

    for (size_t i = 0; i < nibbles.size(); ++i)
    {
        decoded[i + 1] = decoder.decodeNibble(nibbles[i]);
    }

    return decoded;
}

void dumpRaw(std::vector<uint8_t>& rawData)
{
    FILE* fp = fopen("dump.raw", "wb");
    fwrite(rawData.data(), rawData.size(), 1, fp);
    fclose(fp);
}


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
 * by 16. A value between 3 and 5 produces good results.
 * Increasing the number further does not add much quality improvements,
 * but drastically increases the runtime, so 5 is the default.
 */
#if 1
std::vector<uint8_t> createAdpcm4BitFromRawOpenMP(const std::vector<uint8_t>& raw, uint64_t combinedNibbles = 5)
{
    uint64_t squaredSum = 0u;

    CreativeAdpcmDecoder4Bit decoder(raw[0]);
    
    std::vector<uint64_t> result(raw.size() / combinedNibbles);

    for (size_t i = 1; i < raw.size() / combinedNibbles; ++i)
    {
        std::vector<Best> bestResults(omp_get_max_threads());

        // try every possible input for the decoder
        #pragma omp parallel for
        for (uint64_t n = 0; n < constPow(16, combinedNibbles); ++n)
        {
            CreativeAdpcmDecoder4Bit decoderCopy = decoder;
            uint64_t diffSum = 0;
            for (size_t nib = 0; nib < combinedNibbles; ++nib)
            {
                int32_t diff = (int32_t)decoderCopy.decodeNibble(getNthNibble(nib, n)) - (int32_t)raw[i*combinedNibbles + nib - combinedNibbles + 1];
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


        // if (i % 10 == 0) printf("%d\n", i);
    }

    // printf("sum: %ld\n", squaredSum);

    std::vector<uint8_t> nibbles(result.size() * combinedNibbles);
    for (size_t i = 0; i < result.size(); ++i)
    {
        for (size_t nib = 0; nib < combinedNibbles; ++nib)
        {
            nibbles[i * combinedNibbles + nib] = getNthNibble(nib, result[i]);
        }
    }

    // auto decoded = decodeAdpcm4(raw[0], nibbles);
    // dumpRaw(decoded);

    std::vector<uint8_t> binaryResult(nibbles.size() / 2);

    // merge nibbles into bytes
    for (size_t n = 0; n < nibbles.size() / 2; ++n)
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

    for (size_t i = 1; i < raw.size() / combinedNibbles; ++i)
    {
        Best bestResults;

        // try every possible input for the decoder
        for (uint64_t n = 0; n < constPow(16, combinedNibbles); ++n)
        {
            CreativeAdpcmDecoder4Bit decoderCopy = decoder;
            uint64_t diffSum = 0;
            for (size_t nib = 0; nib < combinedNibbles; ++nib)
            {
                int diff = decoderCopy.decodeNibble(getNthNibble(nib, n)) - raw[i*combinedNibbles + nib - combinedNibbles + 1];
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

    printf("sum: %ld\n", squaredSum);

    std::vector<uint8_t> nibbles(result.size() * combinedNibbles);
    for (size_t i = 0; i < result.size(); ++i)
    {
        for (size_t nib = 0; nib < combinedNibbles; ++nib)
        {
            nibbles[i * combinedNibbles + nib] = getNthNibble(nib, result[i]);
        }
    }

    std::vector<uint8_t> binaryResult(nibbles.size() / 2);

    // merge nibbles into bytes
    for (size_t n = 0; n < nibbles.size() / 2; ++n)
    {
        binaryResult[n] = ((nibbles[2 * n] << 4) + (nibbles[2 * n + 1]));
    }

    binaryResult.insert(binaryResult.begin(), raw[0]);

    return binaryResult;
}


struct BestStep
{
    uint8_t accumulator;
    uint8_t previous;
    size_t squaredDiff;
    uint64_t history;
};


void calculateStepRecursively(const uint8_t *data, uint8_t accumulator, uint8_t previous, size_t squaredDiff, uint64_t history, size_t recursionDepth, BestStep& bestStep)
{
    Vec16uc accumulators(accumulator);
    Vec16uc previousValues(previous);
    calculateAllNibbles(previousValues, accumulators);
    nibbleDecodings+=16;

    Vec16uc diff = select(previousValues > *data, previousValues - (*data), (*data) - previousValues);

    if (recursionDepth != 0)
    {
        for (size_t i = 0; i < 16; ++i)
        {
            calculateStepRecursively(data + 1, accumulators[i], previousValues[i], squaredDiff + diff[i] * diff[i], history | (i << (4 * recursionDepth)), recursionDepth - 1, bestStep);
        }
    }
    else
    {
        // Vec16us cDiff = extend(diff);
        // Vec16us squaredDiffs = diff

        for (size_t i = 0; i < 16; ++i)
        {
            auto currentDiff = squaredDiff + diff[i] * diff[i];
            if (currentDiff < bestStep.squaredDiff)
            {
                bestStep.squaredDiff = currentDiff;
                bestStep.accumulator = accumulators[i];
                bestStep.previous = previousValues[i];
                bestStep.history = history | (i << (4 * recursionDepth));
            }
        }
    }
}

std::vector<uint8_t> createAdpcm4BitFromRawSIMD(const std::vector<uint8_t>& raw, [[maybe_unused]] uint64_t combinedNibbles = 4)
{
    uint64_t squaredSum = 0u;
    std::vector<uint8_t> nibbles;
    nibbles.reserve(raw.size());

    BestStep bestStep;
    bestStep.accumulator = 1;
    bestStep.previous = raw[0];
    bestStep.squaredDiff = std::numeric_limits<size_t>::max();

    for (size_t i = 1; i < raw.size() - combinedNibbles; i += combinedNibbles)
    {
        Vec16uc accumulators(bestStep.accumulator);
        Vec16uc previous(bestStep.previous);
        bestStep.squaredDiff = std::numeric_limits<size_t>::max();
        calculateStepRecursively(&raw[i], accumulators[0], previous[0], 0, 0, combinedNibbles - 1, bestStep);

        for (int n = combinedNibbles - 1; n >= 0; --n)
        {
            nibbles.push_back((bestStep.history >> (4 * n)) & 0xf);
        }

        squaredSum += bestStep.squaredDiff;
    }

    std::cout << "SIMD sum: " << squaredSum << "\n";

    std::vector<uint8_t> binaryResult(nibbles.size() / 2);

    // merge nibbles into bytes
    for (size_t n = 0; n < nibbles.size() / 2; ++n)
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


uint64_t calcTotalSquareError(const std::vector<uint8_t>& adpcm, const std::vector<uint8_t>& raw)
{
    std::vector<uint8_t> nibbles((adpcm.size() - 1) * 2);
    for (size_t i = 1; i < adpcm.size(); ++i)
    {
        nibbles[2 * (i - 1)] = adpcm[i] >> 4;
        nibbles[2 * (i - 1) + 1] = adpcm[i] & 0xf;
    }
    auto decoded = decodeAdpcm4(adpcm[0], nibbles);

    uint64_t squaredSum = 0;
    for (size_t i = 0; i < decoded.size(); ++i)
    {
        int diff = decoded[i] - raw[i];
        squaredSum += diff * diff;
    }

    storeFile("dump_raw.raw", raw);
    storeFile("dump_adpcm.raw", decoded);

    return squaredSum;
}

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
                std::cout << "raw size: " << raw.size() << "\n";
                sampleData = createAdpcm4BitFromRawSIMD(raw,6);
                std::cout << "total error: " << calcTotalSquareError(sampleData, raw) << "\n";

                break;
            }
            case VOC_FORMAT_PCM_8BIT:
            {
                sampleData = loadFile(parser.getValue<std::string>("input"));
                break;
            }
            case VOC_FORMAT_ADPCM_3BIT:
            case VOC_FORMAT_ADPCM_2BIT:
            {
                printf("compression format not implemented\n");
                return 1;
            }
        }

        std::vector<uint8_t> vocData = createVocFile(parser.getValue<uint32_t>("frequency"), sampleData, format);

        storeFile(parser.getValue<std::string>("output"), vocData);

        std::cout << "nibble decodings: " << nibbleDecodings << "\n";

        return 0;
    }
    catch (const std::exception &e)
    {
        printf("Error: %s\n", e.what());
        return 1;
    }
}
