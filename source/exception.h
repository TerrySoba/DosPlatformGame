#ifndef _EXCEPTION_H_INCLUDED
#define _EXCEPTION_H_INCLUDED

class Exception
{
public:
    Exception(const Exception& other);
    Exception(const char* msg);
    Exception(const char* msg1, const char* msg2);

    const char* what() const;

    ~Exception();

private:
    char* m_msg;
};

#endif