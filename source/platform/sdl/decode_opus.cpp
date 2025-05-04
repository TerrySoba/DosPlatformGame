#include "decode_opus.h"

#include <opusfile.h>

#include "exception.h"

#include <iostream>
#include <vector>
#include <fstream>

OpusDecoder::OpusDecoder(const std::string& filename) :
    m_filename(filename)
{
    rewind();
}

void OpusDecoder::rewind()
{
    m_opusFile = std::shared_ptr<OggOpusFile>(op_open_file(m_filename.c_str(), nullptr), op_free);
    if (!m_opusFile) {
        throw Exception("Failed to open Opus file: ", m_filename.c_str());
    }
}

uint32_t OpusDecoder::decode(int16_t* outputBuffer, uint32_t bufferSize)
{
    return op_read_stereo(m_opusFile.get(), outputBuffer, bufferSize);
}



// bool decodeOpusFile(const std::string& inputFilePath, const std::string& outputFilePath) {
//     OggOpusFile* opusFile = op_open_file(inputFilePath.c_str(), nullptr);
//     if (!opusFile) {    
//         std::cerr << "Failed to open Opus file: " << inputFilePath << std::endl;
//         return false;
//     }

//     std::ofstream outputFile(outputFilePath, std::ios::binary);
//     if (!outputFile.is_open()) {
//         std::cerr << "Failed to open output file: " << outputFilePath << std::endl;
//         op_free(opusFile);
//         return false;
//     }

//     const int bufferSize = 960 * 2 * 2; // 960 samples, 2 channels, 16-bit PCM
//     std::vector<opus_int16> pcmBuffer(bufferSize);
//     int currentSection;
//     int samplesRead;

//     while ((samplesRead = op_read_stereo(opusFile, pcmBuffer.data(), bufferSize)) > 0) {
//         outputFile.write(reinterpret_cast<const char*>(pcmBuffer.data()), samplesRead * sizeof(opus_int16));
//     }

//     if (samplesRead < 0) {
//         std::cerr << "Error decoding Opus file: " << inputFilePath << std::endl;
//         op_free(opusFile);
//         return false;
//     }

//     op_free(opusFile);
//     outputFile.close();
//     return true;
// }