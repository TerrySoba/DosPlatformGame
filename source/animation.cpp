#include "animation.h"
#include "json.h"
#include "exception.h"
#include "log.h"

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


Animation::Animation(const char* jsonFilename, const char* tgaFilename, bool transparent) :
    m_transparent(transparent)
{
    TgaImage image(tgaFilename);
    Filename filename(jsonFilename);
    Json json(filename);
    JsonValue value = json.getRoot();
    if (!value.isObject())
    {
        throw Exception("Invalid json file: ", jsonFilename);
    }

    JsonValue frames = value.at("frames");

    for (int i = 0; i < frames.size(); ++i)
    {
        JsonValue frame = frames.at(i).at("frame");
        int x = frame.at("x").toInt();
        int y = frame.at("y").toInt();
        int w = frame.at("w").toInt();
        int h = frame.at("h").toInt();

        int duration = frames.at(i).at("duration").toInt();

        Frame f = {
            x, y, w, h, duration
        };

        m_frames.push_back(f);
        FrameImage frameImage(image, x, y, w, h);
        m_frameSprites.push_back(CompiledSprite::compileSprite(frameImage, 320));
    }

    JsonValue tags = value.at("meta").at("frameTags");
    for (int i = 0; i < tags.size(); ++i)
    {
        JsonValue tag = tags.at(i);
        FrameTag frameTag; //(, , tag.at("name").toString().c_str());
        frameTag.startFrame = tag.at("from").toInt();
        frameTag.endFrame = tag.at("to").toInt();
        frameTag.name = tag.at("name").toString(); //.c_str();
        m_tags.push_back(frameTag);
    }

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
