#ifndef _SAFE_FILE_INCLUDED_
#define _SAFE_FILE_INCLUDED_

#include <stdio.h>

#include "exception.h"

class SafeFile
{
public:
    SafeFile(const char *pathname, const char *mode)
    {
        m_fp = fopen(pathname, mode);
        if (!m_fp)
        {
            throw Exception("Could not open file:", pathname);
        }
    }

    ~SafeFile()
    {
        fclose(m_fp);
    }

    FILE* file()
    {
        return m_fp;
    }

private:
    FILE* m_fp;
};

#endif