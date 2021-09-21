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
