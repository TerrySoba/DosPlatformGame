#ifndef VGAGFX_H_INCLUDED
#define VGAGFX_H_INCLUDED

#include "vector.h"

#include "image_base.h"
#include "drawable.h"
#include "rectangle.h"


class VgaGfx
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
	
private:
	char* m_backgroundImage;
	char* m_screenBuffer;
	tnd::vector<Rectangle> m_undrawnRects;
	tnd::vector<Rectangle> m_dirtyRects;
};


#endif
