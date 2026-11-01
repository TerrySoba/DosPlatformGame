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

int findIntersectingRect(const Rectangle* rects, int count, int start, const RectBounds& bounds)
{
	// local copies, so that the compiler does not need to reload them through the far pointer
	int left = bounds.left;
	int top = bounds.top;
	int right = bounds.right;
	int bottom = bounds.bottom;

	const Rectangle* rect = rects + start;
	for (int i = start; i < count; ++i, ++rect)
	{
		// cheapest tests first, most rectangles are rejected by the first one
		if (rect->x > right) continue;
		if (rect->y > bottom) continue;
		if (rect->x + rect->width < left) continue;
		if (rect->y + rect->height < top) continue;
		return i;
	}
	return -1;
}
