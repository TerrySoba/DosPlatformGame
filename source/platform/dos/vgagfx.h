#ifndef VGAGFX_H_INCLUDED
#define VGAGFX_H_INCLUDED

#include "vector.h"

#include "image_base.h"
#include "drawable.h"
#include "rectangle.h"
#include "gfx_output.h"

#include <stdint.h>

class VgaGfx : public GfxOutput
{
public:
	VgaGfx();
	virtual ~VgaGfx();

	void drawScreen();

	void setBackground(const ImageBase& image);

	void draw(const Drawable& image, int16_t x, int16_t y);
	void drawBackground(const Drawable& image, int16_t x, int16_t y);

	void vsync();

	void clear();
	
	void saveAsTgaImage(const char* filename);

	void renderToMemory(void* buffer, uint32_t pitch, PixelFormat format) {}

	uint32_t drawDeathEffect();

	void fancyWipe(const ImageBase& image);

private:
	char* m_backgroundImage;
	char* m_screenBuffer;
	tnd::vector<Rectangle> m_undrawnRects;
	tnd::vector<Rectangle> m_dirtyRects;

	// variables needed to the death effect
	uint8_t m_greyFramesLeft;
	uint8_t* m_greyPaletteAnimation;
};


#endif
