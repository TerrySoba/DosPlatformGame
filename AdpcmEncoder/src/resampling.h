#ifndef RESAMPLING_H
#define RESAMPLING_H

#include <cstdint>
#include <vector>

std::vector<float> resample(const std::vector<float>& inputData, uint32_t inputSampleRate, uint32_t outputSampleRate);

std::vector<float> toFloatVector(const std::vector<int32_t>& input);
std::vector<float> toFloatVector(const std::vector<int16_t>& input);
std::vector<float> toFloatVector(const std::vector<uint8_t>& input);
std::vector<uint8_t> toUint8Vector(const std::vector<float>& input);
std::vector<int16_t> toInt16Vector(const std::vector<float>& input);

void normalize(std::vector<float>& input, float fraction);
void normalizeSumToOne(std::vector<float>& input);


#endif
