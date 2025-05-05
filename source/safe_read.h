#ifndef _SAFE_READ_H_
#define _SAFE_READ_H_

#include "exception.h"


#ifdef __WATCOMC__
    #define safeRead(target, size, n, stream) fread(target, size, n, stream)
#else
    #define safeRead(target, size, n, stream) \
        { \
            if (fread(target, size, n, stream) != n) { \
                THROW_EXCEPTION("Read error, number of read bytes did not match requested number."); \
            } \
        }
    
#endif

#endif
