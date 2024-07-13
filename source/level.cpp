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
    LAYER_BOSS1    = 7,
    LAYER_PLAY_TIME = 8,
    LAYER_MUSIC    = 9,
    LAYER_BOSS2    = 10,
    LAYER_TILESET  = 11,
    LAYER_EYE      = 12,
    LAYER_CUTSCENE = 13,
};


void readDataLayer(FILE* fp, int16_t& layerWidth, int16_t& layerHeight, tnd::vector<uint8_t>& layerData)
{
    fread(&layerWidth, sizeof(layerWidth), 1, fp);
    fread(&layerHeight, sizeof(layerHeight), 1, fp);
    layerData.resize(layerWidth * layerHeight);
    fread(layerData.data(), layerWidth * layerHeight, 1, fp);
}

void readRectangleLayer(FILE* fp, Rectangle& rect, int16_t offsetX, int16_t offsetY)
{
    uint16_t x,y,w,h;
    fread(&x, sizeof(x), 1, fp);
    fread(&y, sizeof(y), 1, fp);
    fread(&w, sizeof(w), 1, fp);
    fread(&h, sizeof(h), 1, fp);
    rect = Rectangle(x+offsetX, y+offsetY, w, h);
}

void Level::setTilesImage(tnd::shared_ptr<ImageBase> tilesImage)
{
    m_tilesImage = tilesImage;
}

Level::Level(const char* mapFilename,
             int16_t tileWidth, int16_t tileHeight,
             int16_t offsetX, int16_t offsetY) :
    m_tileWidth(tileWidth),
    m_tileHeight(tileHeight),
    m_offsetX(offsetX),
    m_offsetY(offsetY),
    m_guffinGate(10),
    m_sun(-1, -1),
    m_musicIndex(0),
    m_cutscene(0)
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
        fclose(fp);
        throw Exception("Incorrect map header", mapFilename);
    }

    uint16_t layerCount;
    fread(&layerCount, sizeof(layerCount), 1, fp);

    tnd::vector<uint8_t> collisionData;
    int16_t collisionWidth = 0;
    int16_t collisionHeight = 0;

    for (int i = 0; i < layerCount; ++i)
    {
        uint8_t layerType;
        fread(&layerType, sizeof(layerType), 1, fp);
        uint16_t layerDataSize;
        fread(&layerDataSize, sizeof(layerDataSize), 1, fp);

        Rectangle rect;
        switch(layerType)
        {
            case LAYER_BG:
            {
                readDataLayer(fp, m_mapWidth, m_mapHeight, m_mapData);
                break;
            }
            case LAYER_COL:
            {
                readDataLayer(fp, collisionWidth, collisionHeight, collisionData);
                break;
            }
            case LAYER_TEXT:
            {
                uint16_t textId;
                fread(&textId, sizeof(textId), 1, fp);
                readRectangleLayer(fp, rect, offsetX, offsetY);
                m_messageBoxes.push_back(MessageBox(textId, rect.x, rect.y, rect.width, rect.height));
                break;
            }
            case LAYER_FIREBALL:
            {
                readRectangleLayer(fp, rect, offsetX, offsetY);
                rect *= 16;
                m_fireBalls.push_back(rect);
                break;
            }
            case LAYER_SEEKER:
            {
                readRectangleLayer(fp, rect, offsetX, offsetY);
                rect *= 16;
                m_seekerEnemies.push_back(rect);
                break;
            }
            case LAYER_EYE:
            {
                readRectangleLayer(fp, rect, offsetX, offsetY);
                rect *= 16;
                m_eyes.push_back(rect);
                break;
            }
            case LAYER_GUFFIN_GATE:
            {
                fread(&m_guffinGate, sizeof(m_guffinGate), 1, fp);
                break;
            }
            case LAYER_BOSS1:
            {
                readRectangleLayer(fp, rect, offsetX, offsetY);
                rect *= 16;
                m_boss1.push_back(rect);
                break;
            }
            case LAYER_BOSS2:
            {
                readRectangleLayer(fp, rect, offsetX, offsetY);
                rect *= 16;
                m_boss2.push_back(rect);
                break;
            }
            case LAYER_PLAY_TIME:
            {
                readRectangleLayer(fp, rect, offsetX, offsetY);
                m_playTime.push_back(rect);
                break;
            }
            case LAYER_MUSIC:
            {
                fread(&m_musicIndex, sizeof(m_musicIndex), 1, fp);
                break;
            }
            case LAYER_CUTSCENE:
            {
                fread(&m_cutscene, sizeof(m_cutscene), 1, fp);
                break;
            }
            case LAYER_TILESET:
            {
                uint16_t tilesetFilenameLength;
                char tilesetFilename[13]; // 8.3 filename + null terminator
                fread(&tilesetFilenameLength, sizeof(tilesetFilenameLength), 1, fp);
                if (tilesetFilenameLength > 12)
                {
                    throw Exception("Tileset filename too long", mapFilename);
                }
                fread(tilesetFilename, tilesetFilenameLength, 1, fp);
                tilesetFilename[tilesetFilenameLength] = 0;
                m_tileset = tilesetFilename;
            }
            default: // skip unknown layers
            {
                fseek(fp, layerDataSize, SEEK_CUR);
            }
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

        m_tentacles = detectLines(collisionData.data(), collisionWidth, collisionHeight, HORIZONTAL, TILE_TENTACLE);
        for (int i = 0; i < m_tentacles.size(); ++i)
        {
            m_tentacles[i].scale(tileWidth * 16, tileHeight * 16);
            m_tentacles[i] += offset * 16;
        }

        m_tentacleArms = detectLines(collisionData.data(), collisionWidth, collisionHeight, HORIZONTAL, TILE_TENTACLE_ARM);
        for (int i = 0; i < m_tentacleArms.size(); ++i)
        {
            m_tentacleArms[i].scale(tileWidth * 16, tileHeight * 16);
            m_tentacleArms[i] += offset * 16;
        }

        m_jetPacks = detectLines(collisionData.data(), collisionWidth, collisionHeight, HORIZONTAL, TILE_JET_PACK);
        for (int i = 0; i < m_jetPacks.size(); ++i)
        {
            m_jetPacks[i].scale(tileWidth * 16, tileHeight * 16);
            m_jetPacks[i] += offset * 16;
        }

        m_sunItems = detectLines(collisionData.data(), collisionWidth, collisionHeight, HORIZONTAL, TILE_SUN_ITEM);
        for (int i = 0; i < m_sunItems.size(); ++i)
        {
            m_sunItems[i].scale(tileWidth * 16, tileHeight * 16);
            m_sunItems[i] += offset * 16;
        }

        {
            tnd::vector<Rectangle> buttons1_on = detectLines(collisionData.data(), collisionWidth, collisionHeight, HORIZONTAL, TILE_SWITCH_1_ON);
            for (int i = 0; i < buttons1_on.size(); ++i)
            {
                Button button(1, BUTTON_ON, buttons1_on[i]); // id == 1
                m_buttons.push_back(button);
            }
        }

        {
            tnd::vector<Rectangle> buttons1_off = detectLines(collisionData.data(), collisionWidth, collisionHeight, HORIZONTAL, TILE_SWITCH_1_OFF);
            for (int i = 0; i < buttons1_off.size(); ++i)
            {
                Button button(1, BUTTON_OFF, buttons1_off[i]); // id == 1
                m_buttons.push_back(button);
            }
        }

        for (int i = 0; i < m_buttons.size(); ++i)
        {
            m_buttons[i].scale(tileWidth * 16, tileHeight * 16);
            m_buttons[i] += offset * 16;
        }

        // find the spawn point and the sun
        for (int x = 0; x < collisionWidth; ++x)
        {
            for (int y = 0; y < collisionHeight; ++y)
            {
                if (collisionData.data()[x + y*collisionWidth] == TILE_SPAWN_POINT)
                {
                    m_spawn.x = x * tileWidth + m_offsetX;
                    m_spawn.y = y * tileHeight + m_offsetY;
                }

                if (collisionData.data()[x + y*collisionWidth] == TILE_SUN_CENTER)
                {
                    m_sun.x = x * tileWidth + m_offsetX;
                    m_sun.y = y * tileHeight + m_offsetY;
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
