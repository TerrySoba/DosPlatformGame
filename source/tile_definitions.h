#ifndef _INCLUDE_TILE_DEFINITIONS
#define _INCLUDE_TILE_DEFINITIONS

enum
{
    TILE_GROUND = 1,        
    TILE_DEATH = 3,
    TILE_FALL_THROUGH = 4,
    TILE_ENEMY = 5,
    TILE_MAC_GUFFIN = 6,
    TILE_GHOST_GROUND = 7,    // this block is untouchable until 10 guffins have been collected, then it becomes like a ground tile
    TILE_MESSAGE_1 = 20,
};

enum
{
    GFX_TILE_GHOST_GROUND = 7, 
};



#endif