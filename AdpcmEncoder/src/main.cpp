#include "file_tools.h"
#include "command_line_parser.h"
#include "voc_format.h"
#include "encode_creative_adpcm_simd.h"
#include "read_wave.h"
#include "resampling.h"

#include <iostream>
#include <map>

std::map <std::string, VocSampleFormat> compressionFormats =
{
    {"PCM", VOC_FORMAT_PCM_8BIT},
    {"ADPCM4", VOC_FORMAT_ADPCM_4BIT},
};

int main(int argc, char* argv[])
{
    try
    {
        clp::CommandLineParser parser(
            "Program to convert WAVE files into VOC files including optional ADPCM compression.\n"
            "File is converted to mono. If a frequency is give then the file is also resampled\n"
            "to the given frequency. Otherwise the sample frequency of the WAVE file is kept.\n\n"
            "Compression formats:\n"
            "  PCM    - unsigned integer 8-bit per sample\n"
            "  ADPCM4 - ADPCM 4-bit per sample\n");
        parser.addParameter("input", "i", "Name of the input file", clp::ParameterRequired::yes);
        parser.addParameter("frequency", "f", "Frequency of output file in hertz", clp::ParameterRequired::no, "-1");
        parser.addParameter("output", "o", "Name of the output file", clp::ParameterRequired::yes);
        parser.addParameter("compression", "c", "Compression to be used. Options: PCM, ADPCM4", clp::ParameterRequired::no, "ADPCM4");
        parser.addParameter("normalize", "n", "Normalize audio to given fraction, e.g. 0.9", clp::ParameterRequired::no, "-1.0");
        parser.addParameter("level", "l", "Level of compression. Must be integer. 1 = lowest quality but fast. Bigger values than 5 probably make no sense and are terribly slow.", clp::ParameterRequired::no, "4");

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

        int32_t targetSampleRate = parser.getValue<int32_t>("frequency");
        auto filename = parser.getValue<std::string>("input");
        auto waveFile = loadWaveFileToMono(filename.c_str());

        printf("Creating file %s, ", parser.getValue<std::string>("output").c_str());

        if (targetSampleRate > 0)
        {
            printf("resampling from %d Hz to %d Hz, ", waveFile.sampleRate, targetSampleRate);
            // resample
            waveFile.data = resample(waveFile.data, waveFile.sampleRate, targetSampleRate);
            waveFile.sampleRate = targetSampleRate;
        }

        // if normalize is requested normalize data
        if (parser.getValue<float>("normalize") > 0.0)
        {
            normalize(waveFile.data, parser.getValue<float>("normalize"));
        }

        auto raw = toUint8Vector(waveFile.data);

        std::vector<uint8_t> sampleData;
        switch(format)
        {
            case VOC_FORMAT_ADPCM_4BIT:
            {
                printf("Output format: ADPCM 4-bit\n");
                sampleData = createAdpcm4BitFromRawSIMD(raw, parser.getValue<uint64_t>("level"));
                break;
            }
            case VOC_FORMAT_PCM_8BIT:
            {
                printf("Output format: PCM 8-bit\n");
                sampleData = raw;
                break;
            }
            case VOC_FORMAT_ADPCM_3BIT:
            case VOC_FORMAT_ADPCM_2BIT:
            {
                printf("compression format not implemented\n");
                return 1;
            }
        }
        std::vector<uint8_t> vocData = createVocFile(waveFile.sampleRate, sampleData, format);
        storeFile(parser.getValue<std::string>("output"), vocData);
        return 0;
    }
    catch (const std::exception &e)
    {
        printf("Error: %s\n", e.what());
        return 1;
    }
}