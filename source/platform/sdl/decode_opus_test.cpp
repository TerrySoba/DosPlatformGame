#include "unit_test.h"
#include "i18n.h"

#include "decode_opus.h"

#include "exception.h"

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

TEST(OpusDecoderTest)
{
    const std::string opusFilename = TEST_DATA_DIR "guffin.opus";
    const std::string referenceFilename = TEST_DATA_DIR "guffin.raw";

    // read decodedFilename file into a buffer
    std::ifstream referenceFile(referenceFilename, std::ios::binary);
    ASSERT_TRUE(referenceFile);

    std::vector<uint8_t> referenceBuffer((std::istreambuf_iterator<char>(referenceFile)), std::istreambuf_iterator<char>());
    referenceFile.close();
    ASSERT_TRUE(!referenceBuffer.empty());
    int16_t* referenceBufferPtr = reinterpret_cast<int16_t*>(referenceBuffer.data());

    OpusDecoder decoder(opusFilename);

    std::vector<int16_t> outputBuffer(1024 * 2); // 1024 samples, 2 channels, 16-bit PCM

    decoder.rewind();
    uint32_t decodedSamples = decoder.decode(outputBuffer.data(), outputBuffer.size());

    ASSERT_TRUE(decodedSamples > 0);
    
    // compare the decoded samples with the decodedBuffer
    for (int i = 0; i < decodedSamples * 2; ++i)
    {
        int32_t diff = static_cast<int32_t>(outputBuffer[i]) - static_cast<int32_t>(referenceBufferPtr[i]);
        // std::cout << "diff: " << diff << std::endl;
        ASSERT_TRUE(std::abs(diff) < 13);
    }
}

TEST(OpusDecoderTestException)
{
    ASSERT_THROW(
        OpusDecoder decoder("non_existent.opus"),
        Exception
    );
}

TEST(OpusDecoderTestDecodeOpusFile)
{
    const std::string opusFilename = TEST_DATA_DIR "guffin.opus";
    const std::string referenceFilename = TEST_DATA_DIR "guffin.raw";

    // read decodedFilename file into a buffer
    std::ifstream referenceFile(referenceFilename, std::ios::binary);
    ASSERT_TRUE(referenceFile);

    std::vector<uint8_t> referenceBuffer((std::istreambuf_iterator<char>(referenceFile)), std::istreambuf_iterator<char>());
    referenceFile.close();
    ASSERT_TRUE(!referenceBuffer.empty());
    int16_t* referenceBufferPtr = reinterpret_cast<int16_t*>(referenceBuffer.data());

    SampleData outputBuffer = decodeOpusFile(opusFilename);

    ASSERT_TRUE(!outputBuffer.empty());

    ASSERT_EQ_INT((int)outputBuffer.size(), (int)referenceBuffer.size() / 2);

    // compare the decoded samples with the decodedBuffer
    for (size_t i = 0; i < outputBuffer.size(); ++i)
    {
        int32_t diff = static_cast<int32_t>(outputBuffer[i]) - static_cast<int32_t>(referenceBufferPtr[i]);
        ASSERT_TRUE(std::abs(diff) < 15);
    }
}
