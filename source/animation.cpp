#include "animation.h"
#include "exception.h"
#include "safe_read.h"

#include "frame_image.h"
#include "platform/dos/rle_sprite.h"

#include <stdio.h>
#include <string.h>

Animation::Animation(const char* animFilename, const char* tgaFilename, bool transparent) :
    m_transparent(transparent)
{
#ifndef PLATFORM_DOS
    TgaImage image(tgaFilename);
#endif
    
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

        #ifndef PLATFORM_DOS
        FrameImage frameImage(image, f.x, f.y, f.width, f.height);
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

    #ifdef PLATFORM_DOS
    m_frameSprites.resize(frameNumber);
    
    for (uint16_t i = 0; i < frameNumber; ++i)
    {
        uint16_t compiledSpriteSize;
        safeRead(&compiledSpriteSize, sizeof(uint16_t), 1, fp);

        uint8_t compiledSpriteType;
        safeRead(&compiledSpriteType, sizeof(uint8_t), 1, fp);

        m_spriteFormat = static_cast<SpriteFormat>(compiledSpriteType);

        m_frameSprites[i] = new char[compiledSpriteSize - 1];
        safeRead(m_frameSprites[i], 1, compiledSpriteSize - 1, fp);
    }
    #endif

    fclose(fp);

    m_minFrame = 0;
    m_maxFrame = m_frames.size() - 1;
    m_currentFrame = m_maxFrame;
    nextFrame();
}

Animation::~Animation()
{
#ifdef PLATFORM_DOS
    for (int i = 0; i < m_frameSprites.size(); ++i)
    {
        delete[] m_frameSprites[i];
    }
#endif
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

typedef void (*DrawCompiledSpritePtr)(char* img, int16_t targetWidth);

void Animation::draw(char* target, int16_t targetWidth, int16_t targetHeight, int16_t targetX, int16_t targetY) const
{
    drawFrame(m_currentFrame, target, targetWidth, targetHeight, targetX, targetY);
}

void Animation::drawFrame(int frame, char* target, int16_t targetWidth, int16_t targetHeight, int16_t targetX, int16_t targetY) const
{
#ifdef PLATFORM_DOS
    if (m_spriteFormat == SPRITE_TYPE_COMPILED)
    {
        DrawCompiledSpritePtr drawFunc = (DrawCompiledSpritePtr)m_frameSprites[frame];
        drawFunc(target + targetY * targetWidth + targetX, targetWidth);
    }
    else if (m_spriteFormat == SPRITE_TYPE_RLE)
    {
        drawRleSprite(m_frameSprites[frame], target + targetY * targetWidth + targetX, targetWidth);
    }
#else
    m_frameSprites[frame]->draw(target, targetWidth, targetHeight, targetX, targetY);
#endif
}

// Compiled sprites contain the line width of the screen, so the frames are drawn with it.
#define MASK_BUFFER_WIDTH 320u

// Color index that no sprite uses, pixels that still have it after drawing are transparent.
#define MASK_BACKGROUND_COLOR 0xFF

void Animation::createCollisionMasks()
{
    // Count the mask rows first, so that the row vector is allocated only once.
    int16_t rowCount = 0;
    int16_t maxHeight = 0;
    m_collisionMaskStart.resize(m_frames.size());
    for (int i = 0; i < m_frames.size(); ++i)
    {
        const Frame& frame = m_frames[i];
        if (frame.width > COLLISION_MASK_MAX_WIDTH)
        {
            m_collisionMaskStart[i] = -1;
            continue;
        }
        m_collisionMaskStart[i] = rowCount;
        rowCount += frame.height;
        if (frame.height > maxHeight) maxHeight = frame.height;
    }

    m_collisionMaskRows.resize(rowCount);
    if (rowCount == 0)
    {
        return;
    }

    // Draw each frame onto a background of an unused color, the pixels the sprite wrote are opaque.
    char* buffer = new char[MASK_BUFFER_WIDTH * maxHeight];
    for (int i = 0; i < m_frames.size(); ++i)
    {
        int16_t start = m_collisionMaskStart[i];
        if (start < 0) continue;

        const Frame& frame = m_frames[i];
        memset(buffer, MASK_BACKGROUND_COLOR, MASK_BUFFER_WIDTH * frame.height);
        drawFrame(i, buffer, MASK_BUFFER_WIDTH, frame.height, 0, 0);

        for (int16_t y = 0; y < frame.height; ++y)
        {
            const uint8_t* line = (const uint8_t*)buffer + y * MASK_BUFFER_WIDTH;
            uint16_t row = 0;
            for (int16_t x = 0; x < frame.width; ++x)
            {
                if (line[x] != MASK_BACKGROUND_COLOR)
                {
                    row |= 0x8000u >> x;
                }
            }
            m_collisionMaskRows[start + y] = row;
        }
    }
    delete[] buffer;
}

CollisionMask Animation::currentCollisionMask() const
{
    CollisionMask mask;
    // useTag() does not update m_width and m_height, so take the size from the frame
    mask.width = m_frames[m_currentFrame].width;
    mask.height = m_frames[m_currentFrame].height;
    mask.rows = 0;

    if (m_currentFrame < m_collisionMaskStart.size())
    {
        int16_t start = m_collisionMaskStart[m_currentFrame];
        if (start >= 0)
        {
            mask.rows = m_collisionMaskRows.data() + start;
        }
    }

    return mask;
}
