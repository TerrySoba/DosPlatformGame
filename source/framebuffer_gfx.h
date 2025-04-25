#ifndef FRAMEBUFFER_GFX_H_INCLUDED
#define FRAMEBUFFER_GFX_H_INCLUDED

#include "vector.h"

#include "image_base.h"
#include "drawable.h"
#include "rectangle.h"
#include "gfx_output.h"

#include <stdint.h>
#include <array>


class FramebufferGfx : public GfxOutput
{
public:
	FramebufferGfx();
	virtual ~FramebufferGfx();

	void drawScreen();

	void setBackground(const ImageBase &image);

	void draw(const Drawable &image, int16_t x, int16_t y);
	void drawBackground(const Drawable &image, int16_t x, int16_t y);

	void vsync();

	void clear();

	void saveAsTgaImage(const char *filename);

	void renderToMemory(void* buffer, uint32_t pitch, PixelFormat format);

	uint32_t drawDeathEffect();

	void fancyWipe(const ImageBase &image);

	void renderToRgb(char* rgbBuffer) const;
	int16_t getScreenWidth() const;
	int16_t getScreenHeight() const;

private:
	char *m_backgroundImage;
	char *m_screenBuffer;
	tnd::vector<Rectangle> m_dirtyRects;
	std::array<uint32_t, 32> m_rgbiColorsToRgba8888;
	uint32_t m_deathEffectFramesLeft = 0;
};

#endif
