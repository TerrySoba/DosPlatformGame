#ifndef FRAMEBUFFER_GFX_H_INCLUDED
#define FRAMEBUFFER_GFX_H_INCLUDED

#include "vector.h"

#include "image_base.h"
#include "drawable.h"
#include "rectangle.h"
#include "gfx_output.h"

#include <stdint.h>

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

	void drawDeathEffect();

	void fancyWipe(const ImageBase &image);

	void renderToRgb(char* rgbBuffer) const;
	int16_t getScreenWidth() const;
	int16_t getScreenHeight() const;

private:
	char *m_backgroundImage;
	char *m_screenBuffer;
	tnd::vector<Rectangle> m_dirtyRects;
};

#endif
