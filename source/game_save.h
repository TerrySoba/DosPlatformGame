#ifndef _INCLUDE_GAME_SAVE_H
#define _INCLUDE_GAME_SAVE_H

#include "game.h"


struct GameState
{
    GameState() :
        jetpackCollected(0)
    {
    }

    LevelNumber level;
    Point spawnPoint;
    tnd::vector<CollectedGuffin> colectedGuffins;
    uint8_t jetpackCollected; // 0 == no jetpack, 1 == jetpack collected
};

void saveGameState(const GameState& gameState, const char* filename);
bool loadGameState(GameState& gameState, const char* filename);

#endif