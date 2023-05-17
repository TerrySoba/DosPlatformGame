#ifndef _INCLUDE_TILE_DEFINITIONS
#define _INCLUDE_TILE_DEFINITIONS

enum
{
    TILE_GROUND = 1,
    TILE_SPAWN_POINT = 2,
    TILE_DEATH = 3,
    TILE_FALL_THROUGH = 4,
    TILE_ENEMY = 5,
    TILE_MAC_GUFFIN = 6,
    TILE_GHOST_GROUND = 7,    // this block is untouchable until enough guffins have been collected, then it becomes like a ground tile
    TILE_JET_PACK = 8,
    TILE_SWITCH_1_ON = 9,
    TILE_SWITCH_1_OFF = 10,
    TILE_SUN_CENTER = 11,
};

enum
{
    GFX_TILE_GHOST_GROUND_1 = 7,
    GFX_TILE_GHOST_GROUND_2 = 97,
    GFX_TILE_SWITCH_1_ON    = 27,
    GFX_TILE_SWITCH_1_OFF   = 29,
};



#endif