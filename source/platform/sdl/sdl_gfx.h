#ifndef SDL_GFX_H_INCLUDED
#define SDL_GFX_H_INCLUDED

#include "gfx_output.h"
#include "framebuffer_gfx.h"

#include "shared_ptr.h"

class SdlGfx : public GfxOutput
{
public:
    SdlGfx(tnd::shared_ptr<FramebufferGfx> framebufferGfx);
	void drawScreen();
	void setBackground(const ImageBase& image);
	void draw(const Drawable& image, int16_t x, int16_t y);
	void drawBackground(const Drawable& image, int16_t x, int16_t y);
	void vsync();
	void clear();
	void saveAsTgaImage(const char* filename);
	void renderToMemory(void* buffer, uint32_t pitch, PixelFormat format);
	uint32_t drawDeathEffect();
	void fancyWipe(const ImageBase& image);

private:
    tnd::shared_ptr<FramebufferGfx> m_framebufferGfx;
};

#endif