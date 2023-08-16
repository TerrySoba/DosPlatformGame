#ifndef _ANIMATION_H_INC_
#define _ANIMATION_H_INC_

#include "tga_image.h"
#include "image.h"
#include "drawable.h"
#include "compiled_sprite.h"

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

class Animation : public Drawable
{
public:
    Animation(const char* jsonFilename, const char* tgaFilename, bool transparent = true);
    void nextFrame();
    
    void useTag(const char* name);
    tnd::vector<FrameTag> getTags();
    
    virtual int16_t width() const;
	virtual int16_t height() const;
    virtual void draw(char* target, int16_t targetWidth, int16_t targetHeight, int16_t x, int16_t y) const;
private:
    void useTag(int16_t);

private:

    int16_t m_width;
    int16_t m_height;

    tnd::vector<Frame> m_frames;
    tnd::vector<tnd::shared_ptr<CompiledSprite> > m_frameSprites;
    tnd::vector<FrameTag> m_tags;
    int m_currentFrame;
    int m_minFrame;
    int m_maxFrame;
    bool m_transparent;
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



#endif
