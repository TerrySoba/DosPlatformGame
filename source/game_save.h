#ifndef _INCLUDE_GAME_SAVE_H
#define _INCLUDE_GAME_SAVE_H

#include "game.h"
#include "music_controller.h"

struct GameState
{
    GameState() :
        jetpackCollected(0),
        sunItemCollected(0),
        button1(0),
        deathCounter(0),
        frameCounter(0),
        musicIndex(MUSIC_INDEX_NO_CHANGE),
        storyStatus(STORY_STATUS_INITIAL)
    {
        level.x = 0;
        level.y = 0;
    }

    LevelNumber level;
    Point spawnPoint;
    tnd::vector<CollectedGuffin> colectedGuffins;
    uint8_t jetpackCollected; // 0 == no jetpack, 1 == jetpack collected
    uint8_t sunItemCollected; // 0 == not collected, 1 == collected
    uint8_t button1; // 0 == button not pressed, 1 == button pressed
    uint32_t deathCounter;
    uint32_t frameCounter; // frame of gameplay (usually 70 frames per second)
    SongIndex musicIndex;  // music that was playing when saved
    StoryStatus storyStatus;
};

void saveGameState(const GameState& gameState, const char* filename);
bool loadGameState(GameState& gameState, const char* filename);

#endif