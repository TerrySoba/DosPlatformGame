#include "unit_test.h"
#include "i18n.h"

#include "decode_vorbis.h"

#include "exception.h"

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

TEST(VorbisDecoderTest)
{
    const std::string vorbisFilename = TEST_DATA_DIR "guffin.ogg";
    const std::string referenceFilename = TEST_DATA_DIR "guffin2.raw";

    // read decodedFilename file into a buffer
    std::ifstream referenceFile(referenceFilename, std::ios::binary);
    ASSERT_TRUE(referenceFile);

    std::vector<uint8_t> referenceBuffer((std::istreambuf_iterator<char>(referenceFile)), std::istreambuf_iterator<char>());
    referenceFile.close();
    ASSERT_TRUE(!referenceBuffer.empty());
    int16_t* referenceBufferPtr = reinterpret_cast<int16_t*>(referenceBuffer.data());


    VorbisDecoder decoder(vorbisFilename);

    std::vector<int16_t> tmpBuffer(1024 * 2); // 1024 samples, 2 channels, 16-bit PCM
    std::vector<int16_t> outputBuffer;


    uint32_t decodedSamples = 0;
    do 
    {
        decodedSamples = decoder.decode(tmpBuffer.data(), tmpBuffer.size());
        outputBuffer.insert(outputBuffer.end(), tmpBuffer.begin(), tmpBuffer.begin() + decodedSamples * 2);
    }
    while (decodedSamples > 0);

    ASSERT_EQ_INT(outputBuffer.size() * 2, referenceBuffer.size());

    // Compare the decoded output with the reference buffer
    for (size_t i = 0; i < referenceBuffer.size() / sizeof(int16_t); ++i) {
        int32_t diff = std::abs(referenceBufferPtr[i] - outputBuffer[i]);
        ASSERT_TRUE(diff < 5);
    }
}


TEST(DecodeVorbisFileTest)
{
    const std::string vorbisFilename = TEST_DATA_DIR "guffin.ogg";
    const std::string referenceFilename = TEST_DATA_DIR "guffin2.raw";

    // read decodedFilename file into a buffer
    std::ifstream referenceFile(referenceFilename, std::ios::binary);
    ASSERT_TRUE(referenceFile);

    std::vector<uint8_t> referenceBuffer((std::istreambuf_iterator<char>(referenceFile)), std::istreambuf_iterator<char>());
    referenceFile.close();
    ASSERT_TRUE(!referenceBuffer.empty());
    int16_t* referenceBufferPtr = reinterpret_cast<int16_t*>(referenceBuffer.data());

    SampleData outputBuffer = decodeVorbisFile(vorbisFilename);

    ASSERT_EQ_INT(outputBuffer.size() * 2, referenceBuffer.size());

    // Compare the decoded output with the reference buffer
    for (size_t i = 0; i < referenceBuffer.size() / sizeof(int16_t); ++i) {
        int32_t diff = std::abs(referenceBufferPtr[i] - outputBuffer[i]);
        ASSERT_TRUE(diff < 5);
    }
}
