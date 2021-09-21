#ifndef _INCLUDE_GAME_SAVE_H
#define _INCLUDE_GAME_SAVE_H

#include "game.h"


struct GameState
{
    LevelNumber level;
    Point spawnPoint;
    tnd::vector<CollectedGuffin> colectedGuffins;
};

void saveGameState(const GameState& gameState, const char* filename);
bool loadGameState(GameState& gameState, const char* filename);

#endif