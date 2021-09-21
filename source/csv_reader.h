#ifndef CSV_READER_INCLUDED
#define CSV_READER_INCLUDED

#include <stdio.h>
#include <stdlib.h>

#include "vector.h"
#include "exception.h"
#include "safe_file.h"


template <class ValueT>
class CsvReader
{
public:
    CsvReader(const char* path)
    {
        SafeFile fp(path, "rb");

        tnd::vector<char> buf;

        int width = -1;
        int tokenCount = 0;
        while(!feof(fp.file()))
        {
            char ch = getc(fp.file());
            
            if (ch == ',' || ch == '\n')
            {
                if (buf.size() > 0)
                {
                    buf.push_back(0);

                    int value = atoi(&buf[0]);

                    // printf("value %d: %d\n", buf.size(), value);
                    m_data.push_back(value);
                    ++tokenCount;
                    buf.clear();

                    if (ch == '\n')
                    {
                        if (width == -1) width = tokenCount;
                        else if (width != tokenCount) throw Exception("Invalid CSV format.");
                        tokenCount = 0;
                    }
                }

            } else if (ch > ' ') {
                buf.push_back(ch);
            }
        }

        m_width = width;
        m_height = m_data.size() / m_width;

    }

    ValueT get(int x, int y) { return m_data[x + y * m_width]; }
    tnd::vector<ValueT>& data() { return m_data; }
    
    int width() { return m_width; }
    int height() { return m_height; }

private:
    int m_height, m_width;
    tnd::vector<ValueT> m_data;
};

#endif