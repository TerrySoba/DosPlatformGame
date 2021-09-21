#include "exception.h"

#include "malloc.h"
#include "string.h"

Exception::Exception(const Exception& other)
{
    if (&other == this) return;

    int len = strlen(other.m_msg);
    m_msg = (char*)malloc(len + 1);
    m_msg[len] = 0;
    memcpy(m_msg, other.m_msg, len);
}

Exception::Exception(const char* msg)
{
    int len = strlen(msg);
    m_msg = (char*)malloc(len + 1);
    m_msg[len] = 0;
    memcpy(m_msg, msg, len);
}

Exception::Exception(const char* msg1, const char* msg2)
{
    int len1 = strlen(msg1);
    int len2 = strlen(msg2);
    m_msg = (char*)malloc(len1 + len2 + 1);
    m_msg[len1 + len2] = 0;
    memcpy(m_msg, msg1, len1);
    memcpy(m_msg + len1, msg2, len2);
}

const char* Exception::what() const
{
    return m_msg;
}

Exception::~Exception()
{
    free((void*)m_msg);
}