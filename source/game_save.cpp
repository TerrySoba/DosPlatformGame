#include "game_save.h"
#include "exception.h"

#include <stdio.h>
#include <string.h>

const char* gameSaveHeader = "TnDSaV";

void saveGameState(const GameState& gameState, const char* filename)
{
    FILE* fp = fopen(filename, "wb");
    if (!fp)
    {
        throw Exception("Could not save game.");
    }
    fwrite(gameSaveHeader, strlen(gameSaveHeader), 1, fp);
    fwrite(&gameState.level.x, sizeof(gameState.level.x), 1, fp);
    fwrite(&gameState.level.y, sizeof(gameState.level.y), 1, fp);
    fwrite(&gameState.spawnPoint.x, sizeof(gameState.spawnPoint.x), 1, fp);
    fwrite(&gameState.spawnPoint.y, sizeof(gameState.spawnPoint.x), 1, fp);

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
    fread(buf, strlen(gameSaveHeader), 1, fp);

    // check for save file header
    if (0 != memcmp(buf, gameSaveHeader, strlen(gameSaveHeader)))
    {
        fclose(fp);
        return false;
    }

    fread(&gameState.level.x, sizeof(gameState.level.x), 1, fp);
    fread(&gameState.level.y, sizeof(gameState.level.y), 1, fp);
    fread(&gameState.spawnPoint.x, sizeof(gameState.spawnPoint.x), 1, fp);
    fread(&gameState.spawnPoint.y, sizeof(gameState.spawnPoint.y), 1, fp);


    size_t collectedCount;
    fread(&collectedCount, sizeof(collectedCount), 1, fp);
    gameState.colectedGuffins.clear();
    for (size_t i = 0; i < collectedCount; ++i)
    {
        CollectedGuffin guffin;
        fread(&guffin.level.x, sizeof(guffin.level.x), 1, fp);
        fread(&guffin.level.y, sizeof(guffin.level.y), 1, fp);
        fread(&guffin.pos.x, sizeof(guffin.pos.x), 1, fp);
        fread(&guffin.pos.y, sizeof(guffin.pos.y), 1, fp);

        gameState.colectedGuffins.push_back(guffin);
    }

    return true;
}
