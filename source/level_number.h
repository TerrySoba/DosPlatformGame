#ifndef LEVEL_NUMBER_H_INCLUDED
#define LEVEL_NUMBER_H_INCLUDED

#include <cstdint>

struct LevelNumber
{
	LevelNumber() : x(0), y(0) {}
	LevelNumber(int8_t x, int8_t y) : x(x), y(y) {}

	int8_t x;
	int8_t y;

	bool operator==(const LevelNumber& other) const
	{
		return (x == other.x && y == other.y);
	}
};

#endif
