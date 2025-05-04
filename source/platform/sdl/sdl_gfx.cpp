#include "sdl_gfx.h"

SdlGfx::SdlGfx(tnd::shared_ptr<FramebufferGfx> framebufferGfx) :
    m_framebufferGfx(framebufferGfx)
{
}

void SdlGfx::drawScreen()
{
    m_framebufferGfx->drawScreen();
}

void SdlGfx::setBackground(const ImageBase &image)
{
    m_framebufferGfx->setBackground(image);
}

void SdlGfx::draw(const Drawable &image, int16_t x, int16_t y)
{
    m_framebufferGfx->draw(image, x, y);
}

void SdlGfx::drawBackground(const Drawable &image, int16_t x, int16_t y)
{
    m_framebufferGfx->drawBackground(image, x, y);
}

void SdlGfx::vsync()
{
    m_framebufferGfx->vsync();
}

void SdlGfx::clear()
{
    m_framebufferGfx->clear();
}

void SdlGfx::saveAsTgaImage(const char *filename)
{
    m_framebufferGfx->saveAsTgaImage(filename);
}

void SdlGfx::renderToMemory(void *buffer, uint32_t pitch, PixelFormat format)
{
    m_framebufferGfx->renderToMemory(buffer, pitch, format);
}

uint32_t SdlGfx::drawDeathEffect()
{
    return m_framebufferGfx->drawDeathEffect();
}

void SdlGfx::fancyWipe(const ImageBase &image)
{
    m_framebufferGfx->fancyWipe(image);
}
