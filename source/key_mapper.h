#ifndef KEY_MAPPER_H_
#define KEY_MAPPER_H_

#include <stdint.h>

typedef enum
{
    KEY_UP = 1,
    KEY_DOWN = 2,
    KEY_LEFT = 4,
    KEY_RIGHT = 8,
    KEY_JUMP = 16,
    KEY_ACTION1 = 32,
    KEY_ACTION2 = 64,
    KEY_ACTION3 = 128,
} Key;

typedef uint8_t KeyBits;

class KeyMapper
{
public:
    KeyBits getKeys() const;
};


#endif