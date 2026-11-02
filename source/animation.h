#ifndef _ANIMATION_H_INC_
#define _ANIMATION_H_INC_

#include "tga_image.h"
#include "image.h"
#include "drawable.h"
#include "collision_mask.h"

#ifndef PLATFORM_DOS
#include "sprite.h"
#endif

#include <stdint.h>
#include <string.h>
#include "vector.h"
#include "tiny_string.h"
#include "shared_ptr.h"

struct Frame
{
    int16_t x,y;
    int16_t width, height;
};

struct FrameTag
{
    int16_t startFrame, endFrame;
    TinyString name;
};

enum SpriteFormat
{
    SPRITE_TYPE_COMPILED = 'C',
    SPRITE_TYPE_RLE = 'R'
};


class Animation : public Drawable
{
public:
    Animation(const char* jsonFilename, const char* tgaFilename, bool transparent = true);
    ~Animation();
    void nextFrame();
    
    void useTag(const char* name);
    tnd::vector<FrameTag> getTags();
    
    virtual int16_t width() const;
	virtual int16_t height() const;
    virtual void draw(char* target, int16_t targetWidth, int16_t targetHeight, int16_t x, int16_t y) const;

    /**
     * Creates the collision masks of all frames. Only call this for animations that take part
     * in collisions, the masks need 2 bytes per frame line.
     */
    void createCollisionMasks();

    /**
     * Returns the collision mask of the current frame. Its rows are 0 if
     * createCollisionMasks() was not called or the frame is too wide.
     */
    CollisionMask currentCollisionMask() const;

private:
    void useTag(int16_t);
    void drawFrame(int frame, char* target, int16_t targetWidth, int16_t targetHeight, int16_t x, int16_t y) const;

private:

    int16_t m_width;
    int16_t m_height;

    tnd::vector<Frame> m_frames;
    #ifdef PLATFORM_DOS
    tnd::vector<char*> m_frameSprites;
    #else
    tnd::vector<tnd::shared_ptr<Sprite> > m_frameSprites;
    #endif

    tnd::vector<FrameTag> m_tags;

    // rows of all frame masks, the mask of frame i starts at m_collisionMaskStart[i]
    tnd::vector<uint16_t> m_collisionMaskRows;
    tnd::vector<int16_t> m_collisionMaskStart; // -1 if frame has no mask

    int m_currentFrame;
    int m_minFrame;
    int m_maxFrame;
    bool m_transparent;
    SpriteFormat m_spriteFormat;
};

/// Animation file format
// header: "ANIM" (4bytes)
// frames: uint16_t
//   frame x:      uint16_t
//   frame y:      uint16_t
//   frame width:  uint16_t
//   frame height: uint16_t
// tag count: uint16_t
//   start frame: uint16_t
//   end frame:   uint16_t
//   tag name length: uint16_t
//   tag name: length bytes
// compiled sprite data: count is the same as frames
//   compiled sprite data size: uint16_t
//   compiled sprite data

#endif
