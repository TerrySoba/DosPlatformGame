#ifndef VECTOR_MATH_H_INCLUDED
#define VECTOR_MATH_H_INCLUDED

#include <stdint.h>

template <class T>
T abs(T val)
{
    return val < 0 ? -val : val;
}

template <class T>
T manhattanNorm(T x, T y)
{
    return abs(x) + abs(y);
}


template <class T, class L>
void setVectorLength(T& x, T& y, L length)
{
    T len = manhattanNorm(x, y);
    if (len != 0)
    {
        x = ((int32_t)x * length) / len;
        y = ((int32_t)y * length) / len;
    }
}

template <class T>
void makeOrthogonal(T& x, T& y)
{
    T temp = x;
    x = -y;
    y = temp;
}


#endif