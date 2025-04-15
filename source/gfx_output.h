#ifndef GFX_OUTPUT_H_INCLUDED
#define GFX_OUTPUT_H_INCLUDED

#include "image_base.h"
#include "drawable.h"
#include "rectangle.h"

#include <stdint.h>


class GfxOutput
{
public:
	virtual ~GfxOutput() {};
	virtual void drawScreen() = 0;
	virtual void setBackground(const ImageBase& image) = 0;
	virtual void draw(const Drawable& image, int16_t x, int16_t y) = 0;
	virtual void drawBackground(const Drawable& image, int16_t x, int16_t y) = 0;
	virtual void vsync() = 0;
	virtual void clear() = 0;
	virtual void saveAsTgaImage(const char* filename) = 0;
	virtual void drawDeathEffect() = 0;
	virtual void fancyWipe(const ImageBase& image) = 0;
};


#endif
