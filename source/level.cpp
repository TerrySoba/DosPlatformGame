#include "level.h"
#include "blit.h"
#include "detect_lines.h"
#include "tile_definitions.h"
#include "exception.h"

#include <stdio.h>
#include <string.h>


Level::~Level()
{
}

// This enum needs to be kept in sync with the map in map_tool.py
enum LayerType
{
    LAYER_BG       = 1,
    LAYER_COL      = 2,
    LAYER_TEXT     = 3,
    LAYER_FIREBALL = 4,
    LAYER_SEEKER   = 5,
    LAYER_GUFFIN_GATE = 6,
};


Level::Level(const char* mapFilename, shared_ptr<ImageBase> tilesImage,
             int16_t tileWidth, int16_t tileHeight,
             int16_t offsetX, int16_t offsetY) :
    m_tilesImage(tilesImage),
    m_tileWidth(tileWidth),
    m_tileHeight(tileHeight),
    m_offsetX(offsetX),
    m_offsetY(offsetY),
    m_guffinGate(10)
{
    FILE* fp = fopen(mapFilename, "rb");
    if (!fp)
    {
        throw Exception("Could not open file:", mapFilename);
    }

    const char* header = "MAP";

    char buf[10];
    fread(buf, strlen(header), 1, fp);
    buf[strlen(header)] = 0;

    if (strcmp(buf, header) != 0)
    {
        throw Exception("Incorrect map header", mapFilename);
    }

    uint16_t layerCount;
    fread(&layerCount, sizeof(layerCount), 1, fp);

    tnd::vector<uint8_t> collisionData;
    uint16_t collisionWidth = 0;
    uint16_t collisionHeight = 0;

    for (int i = 0; i < layerCount; ++i)
    {
        uint8_t layerType;
        fread(&layerType, sizeof(layerType), 1, fp);
        uint16_t layerDataSize;
        fread(&layerDataSize, sizeof(layerDataSize), 1, fp);

        uint16_t layerWidth, layerHeight;

        switch(layerType)
        {
            case LAYER_BG:
            {
                fread(&layerWidth, sizeof(layerWidth), 1, fp);
                fread(&layerHeight, sizeof(layerHeight), 1, fp);
                tnd::vector<uint8_t> layerData(layerWidth * layerHeight);
                fread(layerData.data(), layerWidth * layerHeight, 1, fp);
                m_mapData = layerData;
                m_mapWidth = layerWidth;
                m_mapHeight = layerHeight;
                break;
            }
            case LAYER_COL:
            {
                fread(&layerWidth, sizeof(layerWidth), 1, fp);
                fread(&layerHeight, sizeof(layerHeight), 1, fp);
                tnd::vector<uint8_t> layerData(layerWidth * layerHeight);
                fread(layerData.data(), layerWidth * layerHeight, 1, fp);
                collisionData = layerData;
                collisionWidth = layerWidth;
                collisionHeight = layerHeight;
                break;
            }
            case LAYER_TEXT:
            {
                uint16_t textId,x,y,w,h;
                fread(&textId, sizeof(textId), 1, fp);
                fread(&x, sizeof(x), 1, fp);
                fread(&y, sizeof(y), 1, fp);
                fread(&w, sizeof(w), 1, fp);
                fread(&h, sizeof(h), 1, fp);
                m_messageBoxes.push_back(MessageBox(textId, x+offsetX, y+offsetY, w, h));
                break;
            }
            case LAYER_FIREBALL:
            {
                uint16_t x,y,w,h;
                fread(&x, sizeof(x), 1, fp);
                fread(&y, sizeof(y), 1, fp);
                fread(&w, sizeof(w), 1, fp);
                fread(&h, sizeof(h), 1, fp);
                Rectangle rect(x+offsetX, y+offsetY, w, h);
                rect *= 16;
                m_fireBalls.push_back(rect);
                break;
            }
            case LAYER_SEEKER:
            {
                uint16_t x,y,w,h;
                fread(&x, sizeof(x), 1, fp);
                fread(&y, sizeof(y), 1, fp);
                fread(&w, sizeof(w), 1, fp);
                fread(&h, sizeof(h), 1, fp);
                Rectangle rect(x+offsetX, y+offsetY, w, h);

                rect *= 16;
                m_seekerEnemies.push_back(rect);
                break;
            }
            case LAYER_GUFFIN_GATE:
            {
                fread(&m_guffinGate, sizeof(m_guffinGate), 1, fp);
                break;
            }
            default: // skip unknown layers
                fseek(fp, layerDataSize, SEEK_CUR);
        }

    }

    fclose(fp);

    if (collisionData.size() > 0)
    {

        Point offset(m_offsetX, m_offsetY);

        m_walls = detectLines(collisionData.data(), collisionWidth, collisionHeight, HORIZONTAL, TILE_GROUND);
        for (int i = 0; i < m_walls.size(); ++i)
        {
            m_walls[i].scale(tileWidth * 16, tileHeight * 16);
            m_walls[i] += offset * 16;
        }

        m_ghostWalls = detectLines(collisionData.data(), collisionWidth, collisionHeight, HORIZONTAL, TILE_GHOST_GROUND);
        for (int i = 0; i < m_ghostWalls.size(); ++i)
        {
            m_ghostWalls[i].scale(tileWidth * 16, tileHeight * 16);
            m_ghostWalls[i] += offset * 16;
        }

        m_death = detectLines(collisionData.data(), collisionWidth, collisionHeight, HORIZONTAL, TILE_DEATH);
        for (int i = 0; i < m_death.size(); ++i)
        {
            m_death[i].scale(tileWidth * 16, tileHeight * 16);
            m_death[i] += offset * 16;
            m_death[i].shrink(20);
        }

        m_fallThrough = detectLines(collisionData.data(), collisionWidth, collisionHeight, HORIZONTAL, TILE_FALL_THROUGH);
        for (int i = 0; i < m_fallThrough.size(); ++i)
        {
            m_fallThrough[i].scale(tileWidth * 16, tileHeight * 16);
            m_fallThrough[i] += offset * 16;
            m_fallThrough[i].shrink(20);

            m_walls.push_back(m_fallThrough[i]);
        }

        m_enemies = detectLines(collisionData.data(), collisionWidth, collisionHeight, HORIZONTAL, TILE_ENEMY);
        for (int i = 0; i < m_enemies.size(); ++i)
        {
            m_enemies[i].scale(tileWidth * 16, tileHeight * 16);
            m_enemies[i] += offset * 16;
            m_enemies[i].shrink(20);
        }

        m_guffins = detectLines(collisionData.data(), collisionWidth, collisionHeight, HORIZONTAL, TILE_MAC_GUFFIN);
        for (int i = 0; i < m_guffins.size(); ++i)
        {
            m_guffins[i].scale(tileWidth * 16, tileHeight * 16);
            m_guffins[i] += offset * 16;
        }

        for (int x = 0; x < collisionWidth; ++x)
        {
            for (int y = 0; y < collisionHeight; ++y)
            {
                if (collisionData.data()[x + y*collisionWidth] == 2)
                {
                    m_spawn.x = x * tileWidth + m_offsetX;
                    m_spawn.y = y * tileHeight + m_offsetY;
                }
            }
        }

    }

}

int16_t Level::width() const
{
    return m_tileWidth * m_mapWidth;
}

int16_t Level::height() const
{
    return m_tileHeight * m_mapHeight;
}

class MemoryImage : public ImageBase
{
public:
    MemoryImage(int16_t width, int16_t height, char* data) :
        m_width(width), m_height(height), m_data(data)
    {}

    int16_t width() const { return m_width; }
	int16_t height() const { return m_height; }
	char* data() const { return m_data; }

private:
    int16_t m_width;
    int16_t m_height;
    char* m_data;

};


void Level::draw(char* target, int16_t targetWidth, int16_t targetHeight, int16_t x, int16_t y) const
{
    static const int16_t tileSetWidth = 20;
    static const int16_t tileSetHeight = 12;

    MemoryImage targetImage(targetWidth, targetHeight, target);

    for (int i = 0; i < m_mapData.size(); ++i)
    {
        int index = m_mapData[i];
        Rectangle src((index % tileSetWidth) * m_tileWidth, (index / tileSetWidth) * m_tileHeight, m_tileWidth, m_tileHeight);
        Point pos((i % m_mapWidth) * m_tileWidth + x, (i / m_mapWidth) * m_tileHeight + y);
        blit(*m_tilesImage, src, targetImage, pos);
    }
}
