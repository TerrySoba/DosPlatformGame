#include "tiny_string.h"

#include "malloc.h"
#include "string.h"


TinyString::TinyString()
{
    m_data = NULL;
}

TinyString::TinyString(const char* str)
{
    m_data = strdup(str);
}

TinyString::TinyString(const TinyString& other)
{
    m_data = strdup(other.m_data);
}

TinyString::TinyString(int size)
{
    m_data = (char*)malloc(size+1);
    memset(m_data, ' ', size);
    m_data[size] = '\0';
}

bool TinyString::operator==(const char* str)
{
    return strcmp(str, m_data) == 0;
}

bool TinyString::operator==(const TinyString& other)
{
    return strcmp(other.m_data, m_data) == 0;
}


void TinyString::operator=(const char* str)
{
    free((void*)m_data);
    m_data = strdup(str);
}

void TinyString::operator=(const TinyString& other)
{
    free((void*)m_data);
    m_data = strdup(other.m_data);
}

char TinyString::operator[](int index) const
{
    return m_data[index];
}

int TinyString::size() const
{
    return strlen(m_data);
}

const char* TinyString::c_str() const
{
    return m_data;
}

char* TinyString::data()
{
    return m_data;
}

TinyString::~TinyString()
{
    free((void*)m_data);
}

TinyString mergeStrings(const char* a, const char* b)
{
    int lenA = strlen(a);
    int lenB = strlen(b);
    char* buf = new char[lenA + lenB + 1];
    memcpy(buf, a, lenA);
    memcpy(buf + lenA, b, lenB);
    buf[lenA + lenB] = 0;
    TinyString str(buf);
    delete[] buf;
    return str;
}

TinyString operator+(const TinyString& a, const TinyString& b)
{
    return mergeStrings(a.c_str(), b.c_str());
}

TinyString operator+(const TinyString& a, const char* b)
{
    return mergeStrings(a.c_str(), b);
}

TinyString operator+(const char* a, const TinyString& b)
{
    return mergeStrings(a, b.c_str());
}

int intToString(int32_t value, int base, char* buffer, int bufferSize, int minLength, char fillChar)
{
    char* originalBuffer = buffer;
    if (base < 2 || base > 16 || buffer == NULL || bufferSize <= 0)
    {
        return 0;
    }

    // check if we need to add a minus sign
    if (value < 0)
    {
        value = -value;
        *buffer = '-';
        ++buffer;
        --bufferSize;
    }

    // find out number of digits
    int digits = 0;
    {
        uint32_t tmp = value;
        do
        {
            ++digits;
            tmp /= base;
        } while (tmp > 0);
    }

    // check if we need to add fill characters
    if (digits < minLength)
    {
        digits = minLength;
    }

    // check if we have enough space
    if (digits >= bufferSize)
    {
        digits = bufferSize - 1;
    }

    // fill buffer with fill characters
    for (int i = 0; i < digits; ++i)
    {
        buffer[i] = fillChar;
    }

    // convert number to string
    for (int i = digits - 1; i >= 0; --i)
    {
        buffer[i] = "0123456789ABCDEF"[value % base];
        value /= base;
        if (value == 0)
        {
            break;
        }
    }

    // terminate string
    buffer[digits] = '\0';

    return buffer - originalBuffer + digits;
}

void printCh(char ch);
#pragma aux printCh = \
        "mov ah, 02h"   \
        "int 0x21"      \
        parm    [dl]    \
        modify  [ax];

void printStr(const char* str)
{
    while (str && *str)
    {
        printCh(*str);
        ++str;
    }
}

void printStr(const TinyString& str)
{
    printStr(str.c_str());
}