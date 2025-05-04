#ifndef DECODE_OPUS_H
#define DECODE_OPUS_H

#include <cstdint>
#include <string>
#include <memory>

struct OggOpusFile; // Forward declaration of OggOpusFile struct

class OpusDecoder
{
public:
    OpusDecoder(const std::string& filename);
    ~OpusDecoder() = default;

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
    std::shared_ptr<OggOpusFile> m_opusFile;
};



#endif