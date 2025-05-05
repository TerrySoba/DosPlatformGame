#include "game_save.h"
#include "exception.h"
#include "safe_read.h"

#include <stdio.h>
#include <string.h>

const char* gameSaveHeader = "TnDSaV";

void saveGameState(const GameState& gameState, const char* filename)
{
    FILE* fp = fopen(filename, "wb");
    if (!fp)
    {
        THROW_EXCEPTION("Could not save game.");
    }
    fwrite(gameSaveHeader, strlen(gameSaveHeader), 1, fp);
    fwrite(&gameState.level.x, sizeof(gameState.level.x), 1, fp);
    fwrite(&gameState.level.y, sizeof(gameState.level.y), 1, fp);
    fwrite(&gameState.spawnPoint.x, sizeof(gameState.spawnPoint.x), 1, fp);
    fwrite(&gameState.spawnPoint.y, sizeof(gameState.spawnPoint.y), 1, fp);
    fwrite(&gameState.jetpackCollected, sizeof(gameState.jetpackCollected), 1, fp);
    fwrite(&gameState.sunItemCollected, sizeof(gameState.sunItemCollected), 1, fp);
    fwrite(&gameState.button1, sizeof(gameState.button1), 1, fp);
    fwrite(&gameState.deathCounter, sizeof(gameState.deathCounter), 1, fp);
    fwrite(&gameState.frameCounter, sizeof(gameState.frameCounter), 1, fp);
    fwrite(&gameState.musicIndex, sizeof(gameState.musicIndex), 1, fp);
    fwrite(&gameState.storyStatus, sizeof(gameState.storyStatus), 1, fp);

    size_t collectedCount = gameState.colectedGuffins.size();
    fwrite(&collectedCount, sizeof(collectedCount), 1, fp);

    for (size_t i = 0; i < collectedCount; ++i)
    {
        const CollectedGuffin& guffin = gameState.colectedGuffins[i];
        fwrite(&guffin.level.x, sizeof(guffin.level.x), 1, fp);
        fwrite(&guffin.level.y, sizeof(guffin.level.y), 1, fp);
        fwrite(&guffin.pos.x, sizeof(guffin.pos.x), 1, fp);
        fwrite(&guffin.pos.y, sizeof(guffin.pos.y), 1, fp);
    }

    fclose(fp);
}

bool loadGameState(GameState& gameState, const char* filename)
{
    FILE* fp = fopen(filename, "rb");
    if (!fp)
    {
        return false;
    }

    char buf[10];
    safeRead(buf, strlen(gameSaveHeader), 1, fp);

    // check for save file header
    if (0 != memcmp(buf, gameSaveHeader, strlen(gameSaveHeader)))
    {
        fclose(fp);
        return false;
    }

    safeRead(&gameState.level.x, sizeof(gameState.level.x), 1, fp);
    safeRead(&gameState.level.y, sizeof(gameState.level.y), 1, fp);
    safeRead(&gameState.spawnPoint.x, sizeof(gameState.spawnPoint.x), 1, fp);
    safeRead(&gameState.spawnPoint.y, sizeof(gameState.spawnPoint.y), 1, fp);
    safeRead(&gameState.jetpackCollected, sizeof(gameState.jetpackCollected), 1, fp);
    safeRead(&gameState.sunItemCollected, sizeof(gameState.sunItemCollected), 1, fp);
    safeRead(&gameState.button1, sizeof(gameState.button1), 1, fp);
    safeRead(&gameState.deathCounter, sizeof(gameState.deathCounter), 1, fp);
    safeRead(&gameState.frameCounter, sizeof(gameState.frameCounter), 1, fp);
    safeRead(&gameState.musicIndex, sizeof(gameState.musicIndex), 1, fp);
    safeRead(&gameState.storyStatus, sizeof(gameState.storyStatus), 1, fp);

    size_t collectedCount;
    safeRead(&collectedCount, sizeof(collectedCount), 1, fp);
    gameState.colectedGuffins.clear();
    for (size_t i = 0; i < collectedCount; ++i)
    {
        CollectedGuffin guffin;
        safeRead(&guffin.level.x, sizeof(guffin.level.x), 1, fp);
        safeRead(&guffin.level.y, sizeof(guffin.level.y), 1, fp);
        safeRead(&guffin.pos.x, sizeof(guffin.pos.x), 1, fp);
        safeRead(&guffin.pos.y, sizeof(guffin.pos.y), 1, fp);

        gameState.colectedGuffins.push_back(guffin);
    }

    fclose(fp);
    return true;
}
