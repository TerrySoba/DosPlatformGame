#ifndef DECODE_VORBIS_H
#define DECODE_VORBIS_H

#include <cstdint>
#include <string>
#include <memory>
#include <vector>

struct stb_vorbis; // Forward declaration of stb_vorbis struct

class VorbisDecoder
{
public:
    VorbisDecoder(const std::string& filename);
    ~VorbisDecoder() = default;

    /** 
     * Decodes the Opus file and stores the decoded samples in the provided buffer.
     * The output is in 16-bit PCM interleaved stereo format.
     * 
     * @param outputBuffer Pointer to the buffer where the decoded samples will be stored.
     * @param bufferSize Number of 16bit samples that can be stored in the output buffer.
     * @return Returns the number of samples decoded from the Opus file per channel.
     */
    uint32_t decode(int16_t* outputBuffer, uint32_t bufferSize);
    
    /**
     * Resets the decoder to the beginning of the Opus file.
     */
    void rewind();

private:
    std::string m_filename;
    std::shared_ptr<stb_vorbis> m_vorbisFile;
};

using SampleData = std::vector<int16_t>;

SampleData decodeVorbisFile(const std::string& inputFilePath);


#endif