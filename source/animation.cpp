#include "animation.h"
#include "exception.h"

#include <stdio.h>
#include <string.h>

class FrameImage : public PixelSource
{
public:
    FrameImage(TgaImage& img, int16_t x, int16_t y, int16_t w, int16_t h) :
        m_img(img), m_x(x), m_y(y), m_w(w), m_h(h)
    {
    }

    virtual int16_t width() const
    {
        return m_w;
    }

    virtual int16_t height() const
    {
        return m_h;
    }

    virtual char pixel(int16_t x, int16_t y) const
    {
        return m_img.data()[m_img.width() * (m_y + y) + m_x + x];
    }

    virtual char transparentColor() const
    {
        return 0;
    }

private:
    TgaImage& m_img;
    int16_t m_x;
    int16_t m_y;
    int16_t m_w;
    int16_t m_h;
};


Animation::Animation(const char* animFilename, const char* tgaFilename, bool transparent) :
    m_transparent(transparent)
{
    TgaImage image(tgaFilename);
    
    FILE* fp = fopen(animFilename, "rb");
    if (!fp)
    {
        throw Exception("Could not open animation: ", animFilename);
    }

    char header[5];
    header[4] = 0;
    fread(header, 4, 1, fp);
    if (strcmp("ANIM", header) != 0)
    {
        throw Exception("File header incorrect: ", animFilename);
    }

    uint16_t frameNumber;
    fread(&frameNumber, sizeof(uint16_t), 1, fp);

    for (uint16_t i = 0; i < frameNumber; ++i)
    {
        Frame f;

        fread(&f.x, sizeof(uint16_t), 1, fp);
        fread(&f.y, sizeof(uint16_t), 1, fp);
        fread(&f.width, sizeof(uint16_t), 1, fp);
        fread(&f.height, sizeof(uint16_t), 1, fp);

        m_frames.push_back(f);
        FrameImage frameImage(image, f.x, f.y, f.width, f.height);
        m_frameSprites.push_back(CompiledSprite::compileSprite(frameImage, 320));
    }

    uint16_t tagNumber;
    fread(&tagNumber, sizeof(uint16_t), 1, fp);

    for (uint16_t i = 0; i < tagNumber; ++i)
    {
        FrameTag frameTag;

        fread(&frameTag.startFrame, sizeof(uint16_t), 1, fp);
        fread(&frameTag.endFrame, sizeof(uint16_t), 1, fp);

        uint16_t nameLen;
        fread(&nameLen, sizeof(uint16_t), 1, fp);

        char str[64];
        if (nameLen > 63)
        {
            throw Exception("tag name is too long:", animFilename);
        }

        fread(str, 1, nameLen, fp);
        str[nameLen] = 0;

        frameTag.name = str;

        m_tags.push_back(frameTag);
    }

    fclose(fp);

    m_minFrame = 0;
    m_maxFrame = m_frames.size() - 1;
    m_currentFrame = m_maxFrame;
    nextFrame();
}

Animation::~Animation()
{
    for (int i = 0; i < m_frameSprites.size(); ++i)
    {
        CompiledSprite::freeCompiledSprite(m_frameSprites[i]);
    }
}


int16_t Animation::width() const
{
    return m_width;
}

int16_t Animation::height() const
{
    return m_height;
}

void Animation::nextFrame()
{
    m_currentFrame += 1;
    if (m_currentFrame > m_maxFrame) m_currentFrame = m_minFrame;
    m_width = m_frames[m_currentFrame].width;
    m_height = m_frames[m_currentFrame].height;
}

void Animation::useTag(int16_t tag)
{
    if (tag >= m_tags.size()) return;

    m_minFrame = m_tags[tag].startFrame;
    m_maxFrame = m_tags[tag].endFrame;
    m_currentFrame = m_minFrame;
}

void Animation::useTag(const char* name)
{
    for (int i = 0; i < m_tags.size(); ++i)
    {
        if (m_tags[i].name == name)
        {
            useTag(i);
            return;
        }
    }
}

tnd::vector<FrameTag> Animation::getTags()
{
    return m_tags;
}

void Animation::draw(char* target, int16_t targetWidth, int16_t targetHeight, int16_t targetX, int16_t targetY) const
{
    char *dst = target + targetWidth * targetY + targetX;
    m_frameSprites[m_currentFrame](dst);
}
