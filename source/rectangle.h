#ifndef RECTANGLE_H_INCLUDED
#define RECTANGLE_H_INCLUDED

#include <stdint.h>

#define _MY_MIN_INT16 -32768

struct Point
{
	Point() :
		x(0), y(0)
	{}

	Point(int _x, int _y) :
		x(_x), y(_y)
	{}

	bool operator==(const Point& other)
	{
		return (x == other.x && y == other.y);
	}

	int16_t x,y;
};

Point operator+(const Point& a, const Point& b);
Point operator-(const Point& a, const Point& b);
Point operator*(const Point& a, int16_t b);
Point operator*(int16_t b, const Point& a);

#define my_max(x,y) ((x) > (y))?(x):(y)
#define my_min(x,y) ((x) > (y))?(y):(x)

struct Rectangle
{
	Rectangle() :
		x(0), y(0),
		width(0), height(0)
	{}

	Rectangle(int _x, int _y, int _width, int _height) :
		x(_x), y(_y),
		width(_width), height(_height)
	{}

	Rectangle(const Rectangle& other) :
		x(other.x), y(other.y),
		width(other.width), height(other.height)
	{}

	bool operator==(const Rectangle& other) const
	{
		return
			x == other.x &&
			y == other.y &&
			width == other.width &&
			height == other.height;
			
	}

	Rectangle& operator*=(int16_t factor)
	{
		x *= factor;
		y *= factor;
		width *= factor;
		height *= factor;
		return *this;
	}

	Rectangle operator+(const Point& offset) const
	{
		Rectangle r = *this;
		r.x += offset.x;
		r.y += offset.y;
		return r;
	}

	Rectangle operator-(const Point& offset) const
	{
		Rectangle r = *this;
		r.x -= offset.x;
		r.y -= offset.y;
		return r;
	}

	Rectangle& operator+=(const Point& offset)
	{
		x += offset.x;
		y += offset.y;
		return *this;
	}


	void scale(int16_t horizontalFactor, int16_t verticalFactor)
	{
		x *= horizontalFactor;
		y *= verticalFactor;
		width *= horizontalFactor;
		height *= verticalFactor;
	}

	void shrink(int16_t offset)
	{
		x += offset;
		y += offset;
		width -= 2 * offset;
		height -= 2 * offset;
	}

	Rectangle intersection(const Rectangle& other) const
	{
		int16_t left = my_max(x, other.x);
		int16_t top = my_max(y, other.y);
		int16_t right = my_min(x + width, other.x + other.width);
		int16_t bottom = my_min(y + height, other.y + other.height);

		if ((right > left) && (bottom > top))
		{
			// we got a non empty intersection
			return Rectangle(left, top, right - left, bottom - top);
		}
		else
		{
			// intersection is empty
			return Rectangle();
		}
	}

	bool empty() const
	{
		return width == 0 || height == 0;
	}

	int16_t x,y;
	int16_t width, height;
};

#endif
