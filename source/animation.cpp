#include "animation.h"
#include "exception.h"
#include "safe_read.h"

#include "frame_image.h"

#include <stdio.h>
#include <string.h>


Animation::Animation(const char* animFilename, const char* tgaFilename, bool transparent) :
    m_transparent(transparent)
{
    TgaImage image(tgaFilename);
    
    FILE* fp = fopen(animFilename, "rb");
    if (!fp)
    {
        THROW_EXCEPTION("Could not open animation: ", animFilename);
    }

    char header[5];
    header[4] = 0;
    safeRead(header, 4, 1, fp);
    if (strcmp("ANIM", header) != 0)
    {
        fclose(fp);
        THROW_EXCEPTION("File header incorrect: ", animFilename);
    }

    uint16_t frameNumber;
    safeRead(&frameNumber, sizeof(uint16_t), 1, fp);

    for (uint16_t i = 0; i < frameNumber; ++i)
    {
        Frame f;

        safeRead(&f.x, sizeof(uint16_t), 1, fp);
        safeRead(&f.y, sizeof(uint16_t), 1, fp);
        safeRead(&f.width, sizeof(uint16_t), 1, fp);
        safeRead(&f.height, sizeof(uint16_t), 1, fp);

        m_frames.push_back(f);
        FrameImage frameImage(image, f.x, f.y, f.width, f.height);
        #ifdef PLATFORM_DOS
        m_frameSprites.push_back(new CompiledSprite(frameImage, 320));
        #else
        m_frameSprites.push_back(tnd::shared_ptr<Sprite>(new Sprite(frameImage, 320)));
        #endif
    }

    uint16_t tagNumber;
    safeRead(&tagNumber, sizeof(uint16_t), 1, fp);

    for (uint16_t i = 0; i < tagNumber; ++i)
    {
        FrameTag frameTag;

        safeRead(&frameTag.startFrame, sizeof(uint16_t), 1, fp);
        safeRead(&frameTag.endFrame, sizeof(uint16_t), 1, fp);

        uint16_t nameLen;
        safeRead(&nameLen, sizeof(uint16_t), 1, fp);

        char str[64];
        if (nameLen > 63)
        {
            fclose(fp);
            THROW_EXCEPTION("tag name is too long:", animFilename);
        }

        safeRead(str, 1, nameLen, fp);
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
    m_frameSprites[m_currentFrame]->draw(target, targetWidth, targetHeight, targetX, targetY);
}
