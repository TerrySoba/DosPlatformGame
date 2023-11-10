#ifndef DECODE_CREATIVE_ADPCM_H
#define DECODE_CREATIVE_ADPCM_H

#include <vector>
#include <cstdint>
#include <algorithm>

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

#endif
