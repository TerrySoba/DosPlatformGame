#include "rectangle.h"

Point operator+(const Point& a, const Point& b)
{
	return Point(a.x + b.x, a.y + b.y);
}

Point operator-(const Point& a, const Point& b)
{
	return Point(a.x - b.x, a.y - b.y);
}

Point operator*(const Point& a, int16_t b)
{
	return Point(a.x * b, a.y * b);
}

Point operator*(int16_t b, const Point& a)
{
	return Point(a.x * b, a.y * b);
}