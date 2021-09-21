#ifndef _IMAGE_BASE_H_INC_
#define _IMAGE_BASE_H_INC_

#include <stdint.h>

class ImageBase
{
public:
    virtual ~ImageBase() {}
    virtual int16_t width() const = 0;
	virtual int16_t height() const = 0;
	virtual char* data() const = 0;

    inline char* linePtr(int16_t line) const
    {
        return data() + (width() * line);
    }

};

#endif
