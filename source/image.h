#ifndef _IMAGE_H_INC_
#define _IMAGE_H_INC_

#include "image_base.h"

#include <stdint.h>

class Image : public ImageBase
{
public:
    Image(const Image& image);
    Image(const char* filename);
    Image(int width, int height, uint8_t value);

    Image& operator=(const Image& other);

    virtual ~Image();

    virtual int16_t width() const { return m_width; }
	virtual int16_t height() const { return m_height; }
	virtual char* data() const { return m_data; }

private:
    int16_t m_width;
    int16_t m_height;
    char* m_data;
};

#endif
