#include "lodepng/lodepng.h"

#include "tclap/CmdLine.h"

#include <iostream>
#include <exception>
#include <stdio.h>
#include <cmath>
#include <vector>
#include <forward_list>
#include <sstream>
#include <optional>
#include <limits>

template <typename T>
auto square(const T& val) -> decltype(val*val)
{
    return val * val;
}

struct RGB
{
    unsigned char r, g, b;
};

int32_t colorDiff(
        const RGB& color1,
        const RGB& color2)
{
    // very simplistic euclidean distance...
    return square((int32_t)color1.r - (int32_t)color2.r) + square((int32_t)color1.g - (int32_t)color2.g) + square((int32_t)color1.b - (int32_t)color2.b);
}

static const std::vector<RGB> rgbiColors =
{
    {0x00,0x00,0x00},
    {0x00,0x00,0xAA},
    {0x00,0xAA,0x00},
    {0x00,0xAA,0xAA},
    {0xAA,0x00,0x00},
    {0xAA,0x00,0xAA},
    {0xAA,0x55,0x00},
    {0xAA,0xAA,0xAA},
    {0x55,0x55,0x55},
    {0x55,0x55,0xFF},
    {0x55,0xFF,0x55},
    {0x55,0xFF,0xFF},
    {0xFF,0x55,0xFF},
    {0xFF,0x55,0xFF},
    {0xFF,0xFF,0x55},
    {0xFF,0xFF,0xFF},
};


unsigned char getBestRgbiColor(const RGB& color)
{
    char bestIndex = 0;
    int32_t bestDiff = std::numeric_limits<int32_t>::max();
    for (char i = 0; i < rgbiColors.size(); ++i)
    {
        int32_t diff;
        if ((diff = colorDiff(color, rgbiColors[i])) < bestDiff)
        {
            bestIndex = i;
            bestDiff = diff;
        }
    }

    return bestIndex;
}

RGB rgba2rgb(uint32_t rgba)
{
    return RGB{
        (uint8_t)((rgba & 0x000000ff) >> 0),
        (uint8_t)((rgba & 0x0000ff00) >> 8),
        (uint8_t)((rgba & 0x00ff0000) >> 16),
        };
}

struct Image
{
    std::vector<uint8_t> data; // RGBA, 8bit per channel
    unsigned int width, height;
};


Image loadPngImage(const std::string& filename)
{
    Image image;

    auto errorcode = lodepng::decode(image.data, image.width, image.height, filename, LCT_RGBA, 8);

    if (errorcode)
    {
        std::stringstream message;
        message << "Could not load input image. error:" << errorcode;
        throw std::runtime_error(message.str());
    }

    return image;
}

/**
 * Converts the given RGBA image to an indexed image conforming to
 * the RGBI 16color palette.
 * This is the color palette use by EGA and Tandy 1000.
 * VGA also defaults the first 16 colors to this palette.
 */
std::vector<uint8_t> convertToRgbi(const Image& img)
{
    std::vector<uint8_t> rgbiImage;
    rgbiImage.reserve(img.data.size() / 4);

    uint32_t* data = (uint32_t*)img.data.data();

    for (unsigned int y = 0; y < img.height; ++y)
    {
        for (unsigned int x = 0; x < img.width; ++x)
        {
            auto color = rgba2rgb(data[img.width*y + x]);
            unsigned char pixel = getBestRgbiColor(color);
            rgbiImage.push_back(pixel);
        }
    }

    return rgbiImage;
}


void write16bit(uint16_t data, FILE* fp)
{
    fwrite(&data, 2, 1, fp);
}

void write8bit(uint8_t data, FILE* fp)
{
    fwrite(&data, 1, 1, fp);
}


struct Chunk
{
    bool isRleChunk;
    std::vector<uint8_t> data;
};


bool isSameRle(const Chunk& a, const Chunk& b)
{
    if (!a.isRleChunk || !b.isRleChunk) return false;
    if (a.data.size() == 0 || b.data.size() == 0) return false;
    return (a.data[0] == b.data[0]);
}

/**
 * Compress data using run length encoding (RLE).
 */
void doRleCompression(const std::vector<uint8_t>& data, FILE *fp)
{
    std::forward_list<Chunk> chunks;

    // initialize chunks with single bytes of data
    auto chunksIter = chunks.before_begin();
    for (auto ch : data)
    {
        chunksIter = chunks.insert_after(chunksIter, {true, {ch}});
    }

    // Merge runs of same value into chunks
    for (auto it = chunks.begin(); it != chunks.end();)
    {
        auto next = it;
        ++next;
        if (next == chunks.end()) break;

        if (isSameRle(*it, *next) &&
            it->data.size() + next->data.size() < 127) // max chunk size is 127 bytes
        {
            // merge RLE blocks
            it->data.insert(it->data.end(), next->data.begin(), next->data.end());
            chunks.erase_after(it);
        }
        else
        {
            ++it;
        }
    }

    // all chunks that are smaller than 3 elements should not be RLE chunks,
    // so we mark them as not being RLE chunks
    for (auto& chunk : chunks)
    {
        if (chunk.data.size() < 3)
        {
            chunk.isRleChunk = false;
        }
    }

    // now merge non RLE chunks
    for (auto it = chunks.begin(); it != chunks.end();)
    {
        auto next = it;
        ++next;
        if (next == chunks.end()) break;

        if (!it->isRleChunk && !next->isRleChunk &&
            it->data.size() + next->data.size() < 127) // max chunk size is 127 bytes
        {
            // merge chunk blocks
            it->data.insert(it->data.end(), next->data.begin(), next->data.end());
            chunks.erase_after(it);
        }
        else
        {
            ++it;
        }
    }

    // now write chunks to file
    for (auto &chunk : chunks)
    {
        auto header = (chunk.data.size() - 1);
        if (chunk.isRleChunk)
            header |= 0x80;
        write8bit(header, fp);

        if (chunk.isRleChunk)
        {
            write8bit(chunk.data[0], fp);
        }
        else
        {
            fwrite(chunk.data.data(), 1, chunk.data.size(), fp);
        }
    }
}

void convertToTga(const Image& image, std::string outputFile, bool useRleCompression = true)
{
    // std::cout << "w:" << image.width << " h:" << image.height << std::endl;

    auto rgbiImage = convertToRgbi(image);

    FILE* fp = fopen(outputFile.c_str(), "wb");

    write8bit(0, fp); // id length in bytes
    write8bit(1, fp); // color map type (1 == color map / palette present)
    if (useRleCompression)
        write8bit(9, fp); // image type (9 == RLE compressed color-mapped image)
    else
        write8bit(1, fp); // image type (1 == uncompressed color-mapped image)
    write16bit(0, fp); // color map origin
    write16bit(rgbiColors.size(), fp); // color map length
    write8bit(24, fp); // color map depth (bits per palette entry)
    write16bit(0, fp); // x origin
    write16bit(0, fp); // y origin
    write16bit(image.width, fp); // image width
    write16bit(image.height, fp); // image height
    write8bit(8, fp); // bits per pixel
    write8bit((1 << 5), fp); // image descriptor

    // we do not have id data, so do not write any

    // write palette data (3 byte per entry) BGR
    for (auto entry : rgbiColors)
    {
        write8bit(entry.b, fp);
        write8bit(entry.g, fp);
        write8bit(entry.r, fp);
    }

    // write image data
    if (useRleCompression)
    {
        doRleCompression(rgbiImage, fp);
    }
    else
    {
        // write uncompressed
        fwrite(rgbiImage.data(), 1, rgbiImage.size(), fp);
    }
    
    fclose(fp);
}


int main(int argc, char *argv[])
{
    try {

        TCLAP::CmdLine cmd(
            "This tool converts PNG images to TGA images suitable for display on RGBI monitors, e.g. EGA or Tandy1000 line of computers. "
            "The input image is converted to a 16 color image in the process. The TGA image uses the RGBI palette.",
            ' ', "0.1");

        TCLAP::UnlabeledValueArg<std::string> inputPath("input", "The input PNG image to be converted.", true, "", "PNG image");
        cmd.add( inputPath );
        TCLAP::UnlabeledValueArg<std::string> outputPath("output", "The output TGA image.", true, "", "output image");
        cmd.add( outputPath );

        TCLAP::SwitchArg compress("c", "compress", "Use RLE to compress image data.");
        cmd.add(compress);

        cmd.parse(argc, argv);

        auto image = loadPngImage(inputPath.getValue());

        convertToTga(image, outputPath.getValue(), compress.getValue());
    
        return 0;

    } catch (std::exception& ex) {
        std::cerr << "Caught exception: " << ex.what() << std::endl;
        return 2;
    }

}
