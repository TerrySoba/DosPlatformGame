#include "resampling.h"

#include <stdint.h>
#include <limits>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <cmath>

std::vector<float> blackmanWindow(size_t length)
{
    std::vector<float> output;
    output.reserve(length);
    for (size_t i = 0; i < length; ++i)
    {
        output.push_back(0.42 - 0.5 * cos(2.0 * M_PI * i / (length - 1)) + 0.08 * cos(4.0 * M_PI * i / (length - 1)));
    }
    return output;
}

std::vector<float> sinc(float sampleRate, float cutoffFrequency, size_t length)
{
    std::vector<float> output;
    output.reserve(length);
    for (size_t i = 0; i < length; ++i)
    {
        float x = (float)i - (float)length / 2.0;
        if (x == 0)
        {
            output.push_back(2.0 * M_PI * cutoffFrequency / sampleRate);
        }
        else
        {
            output.push_back(sin(2.0 * M_PI * cutoffFrequency * x / sampleRate) / x);
        }
    }
    return output;
}

std::vector<float> multiplyVectors(const std::vector<float>& a, const std::vector<float>& b)
{
    std::vector<float> output;
    output.reserve(a.size());
    for (size_t i = 0; i < a.size(); ++i)
    {
        output.push_back(a[i] * b[i]);
    }
    return output;
}

void normalizeSumToOne(std::vector<float>& input)
{
    float sum = 0;
    for (size_t i = 0; i < input.size(); ++i)
    {
        sum += input[i];
    }

    for (size_t i = 0; i < input.size(); ++i)
    {
        input[i] /= sum;
    }
}

void normalize(std::vector<float>& input, float fraction)
{
    float max = 0;
    for (size_t i = 0; i < input.size(); ++i)
    {
        if (std::abs(input[i]) > max)
        {
            max = std::abs(input[i]);
        }
    }

    max /= fraction;

    for (size_t i = 0; i < input.size(); ++i)
    {
        input[i] /= max;
    }
}

/**
 * @brief Creates a lowpass filter with the given parameters.
 * 
 * @param sampleRate The sample rate of the input signal in Hz.
 * @param cutoffFrequency The cutoff frequency of the filter in Hz.
 * @param transitionBandwidth The transition bandwidth of the filter in Hz.
 * 
 * The created filter needs to be applied to the signal to be filtered using convolution.
 */
std::vector<float> createLowpassFilter(float sampleRate, float cutoffFrequency, float transitionBandwidth)
{
    // calculate filter length
    size_t length = 4 * sampleRate / transitionBandwidth;
    if (length % 2 == 0) ++length;  // ensure length is odd

    // generate sinc
    std::vector<float> mySinc = sinc(sampleRate, cutoffFrequency, length);

    // multiply with window
    std::vector<float> window = blackmanWindow(length);
    std::vector<float> sincWindowed = multiplyVectors(mySinc, window);

    normalizeSumToOne(sincWindowed);
    return sincWindowed;
}

// template <typename SampleType>
// std::vector<SampleType> mergeToMono(const std::vector<SampleType>& input, size_t numChannels)
// {
//     std::vector<SampleType> output;
//     output.reserve(input.size() / numChannels);
//     for (size_t i = 0; i < input.size(); i += numChannels)
//     {
//         uint64_t sample = 0;
//         for (size_t j = 0; j < numChannels; ++j)
//         {
//             sample += input[i + j];
//         }
//         sample /= numChannels;
//         output.push_back(sample);
//     }
//     return output;
// }

std::vector<float> toFloatVector(const std::vector<int32_t>& input)
{
    std::vector<float> output;
    output.reserve(input.size());
    for (size_t i = 0; i < input.size(); ++i)
    {
        output.push_back(std::clamp(input[i] / (double)std::numeric_limits<int32_t>::max(), -1.0, 1.0));
    }
    return output;
}

std::vector<float> toFloatVector(const std::vector<int16_t>& input)
{
    std::vector<float> output;
    output.reserve(input.size());
    for (size_t i = 0; i < input.size(); ++i)
    {
        output.push_back(std::clamp(input[i] / (float)std::numeric_limits<int16_t>::max(), -1.0f, 1.0f));
    }
    return output;
}

std::vector<float> toFloatVector(const std::vector<uint8_t>& input)
{
    std::vector<float> output;
    output.reserve(input.size());
    for (size_t i = 0; i < input.size(); ++i)
    {
        output.push_back(std::clamp((input[i] - 128.0) / 128.0, -1.0, 1.0));
    }
    return output;
}

std::vector<uint8_t> toUint8Vector(const std::vector<float>& input)
{
    std::vector<uint8_t> output;
    output.reserve(input.size());
    for (size_t i = 0; i < input.size(); ++i)
    {
        output.push_back(std::round(std::clamp(input[i] * 128.0 + 128.0, 0.0, 255.0)));
    }
    return output;
}

std::vector<int16_t> toInt16Vector(const std::vector<float>& input)
{
    std::vector<int16_t> output;
    output.reserve(input.size());
    for (size_t i = 0; i < input.size(); ++i)
    {
        output.push_back(std::round(
            std::clamp(
                input[i] * std::numeric_limits<int16_t>::max(),
                (float)std::numeric_limits<int16_t>::min(),
                (float)std::numeric_limits<int16_t>::max())));
    }
    return output;
}


std::vector<float> convolution(const std::vector<float>& input, const std::vector<float>& kernel)
{
    std::vector<float> output;
    output.reserve(input.size() + kernel.size() - 1);
    for (size_t i = 0; i < input.size() + kernel.size() - 1; ++i)
    {
        float sample = 0;
        for (size_t j = 0; j < kernel.size(); ++j)
        {
            if (i >= j && i - j < input.size())
            {
                sample += input[i - j] * kernel[j];
            }
        }
        output.push_back(sample);
    }
    return output;
}

std::vector<float> lowPassFilter(const std::vector<float>& input, float sampleRate, float cutoffFrequency, float transitionBandwidth)
{
    auto filter = createLowpassFilter(sampleRate, cutoffFrequency, transitionBandwidth);
    return convolution(input, filter);
}


std::vector<float> resample(const std::vector<float>& inputData, uint32_t inputSampleRate, uint32_t outputSampleRate)
{
    // first lowpass filter signal to prevent aliasing
    auto input = lowPassFilter(inputData, inputSampleRate, outputSampleRate / 2.0, outputSampleRate / 10.0);
    uint64_t outputSize = (uint64_t)input.size() * (uint64_t)outputSampleRate / (uint64_t)inputSampleRate;
    // std::cout << "outputSize = " << outputSize << "\n";
    std::vector<float> output(outputSize);
    // output.reserve(outputSize);
    for (size_t i = 0; i < outputSize; ++i)
    {
        double inputIndex = (double)i * (double)inputSampleRate / (double)outputSampleRate;
        
        // do linear interpolation
        size_t inputIndexFloor = (size_t)inputIndex;
        size_t inputIndexCeil = inputIndexFloor + 1;
        double inputIndexFraction = inputIndex - inputIndexFloor;
        if (inputIndexCeil >= input.size())
        {
            inputIndexCeil = input.size() - 1;
        }
        float sample = (float)((1.0 - inputIndexFraction) * input[inputIndexFloor] + inputIndexFraction * input[inputIndexCeil]);

        // std::cout << "sample[" << i << "] = " << sample << "\n";


        output[i] = sample;
    }
    return output;
}
