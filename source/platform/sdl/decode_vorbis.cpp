#include "decode_vorbis.h"


#include "exception.h"

#include <iostream>
#include <vector>

#define STB_VORBIS_HEADER_ONLY
#include "3rd_party/stb/stb_vorbis.c"


VorbisDecoder::VorbisDecoder(const std::string& filename) :
    m_filename(filename)
{
    rewind();
}

void VorbisDecoder::rewind()
{
    int error = 0;
    m_vorbisFile = std::shared_ptr<stb_vorbis>(
        stb_vorbis_open_filename(m_filename.c_str(), &error, nullptr),
        stb_vorbis_close);

    if (!m_vorbisFile)
    {
        THROW_EXCEPTION("Could not open file: ", m_filename.c_str());
    }

    auto info = stb_vorbis_get_info(m_vorbisFile.get());

    if (info.sample_rate != 48000)
    {
        THROW_EXCEPTION("Vorbis file must be 48kHz: ", m_filename.c_str());
    }

    // std::cout << "Channels: " << info.channels << std::endl;
    // std::cout << "Sample rate: " << info.sample_rate << std::endl;
    // std::cout << "Setup memory required: " << info.setup_memory_required << std::endl;
    // std::cout << "Setup temp memory required: " << info.setup_temp_memory_required << std::endl;
    // std::cout << "Temp memory required: " << info.temp_memory_required << std::endl;
    // std::cout << "Max frame size: " << info.max_frame_size << std::endl;
}

uint32_t VorbisDecoder::decode(int16_t* outputBuffer, uint32_t bufferSize)
{
    return stb_vorbis_get_samples_short_interleaved(
        m_vorbisFile.get(), 2, outputBuffer, bufferSize);
}

SampleData decodeVorbisFile(const std::string& inputFilePath)
{
    VorbisDecoder decoder(inputFilePath);
    SampleData outputBuffer;

    SampleData tmpBuffer(1024 * 2);
    uint32_t samplesDecoded = 0;
    
    do {
        samplesDecoded = decoder.decode(tmpBuffer.data(), tmpBuffer.size());
        outputBuffer.insert(outputBuffer.end(), tmpBuffer.begin(), tmpBuffer.begin() + samplesDecoded * 2);
    } while (samplesDecoded > 0);

    return outputBuffer;
}

