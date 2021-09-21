#include "level.h"
#include "csv_reader.h"
#include "blit.h"
#include "detect_lines.h"
#include "tile_definitions.h"

Level::~Level()
{
}


Level::Level(const char* mapFilename, const char* groundFilename, shared_ptr<ImageBase> tilesImage,
             int16_t tileWidth, int16_t tileHeight,
             int16_t offsetX, int16_t offsetY) :
    m_tilesImage(tilesImage),
    m_tileWidth(tileWidth),
    m_tileHeight(tileHeight),
    m_offsetX(offsetX),
    m_offsetY(offsetY)
{
    {
        CsvReader<uint8_t> reader(mapFilename);
        m_mapWidth = reader.width();
        m_mapHeight = reader.height();
        m_mapData = reader.data();
    }
    {

        Point offset(m_offsetX, m_offsetY);

        CsvReader<uint8_t> bg(groundFilename);
        m_walls = detectLines(bg, HORIZONTAL, TILE_GROUND);
        for (int i = 0; i < m_walls.size(); ++i)
        {
            m_walls[i].scale(tileWidth * 16, tileHeight * 16);
            m_walls[i] += offset * 16;
        }

        m_death = detectLines(bg, HORIZONTAL, TILE_DEATH);
        for (int i = 0; i < m_death.size(); ++i)
        {
            m_death[i].scale(tileWidth * 16, tileHeight * 16);
            m_death[i] += offset * 16;
            m_death[i].shrink(20);
        }

        m_fallThrough = detectLines(bg, HORIZONTAL, TILE_FALL_THROUGH);
        for (int i = 0; i < m_fallThrough.size(); ++i)
        {
            m_fallThrough[i].scale(tileWidth * 16, tileHeight * 16);
            m_fallThrough[i] += offset * 16;
            m_fallThrough[i].shrink(20);

            m_walls.push_back(m_fallThrough[i]);
        }

        m_enemies = detectLines(bg, HORIZONTAL, TILE_ENEMY);
        for (int i = 0; i < m_enemies.size(); ++i)
        {
            m_enemies[i].scale(tileWidth * 16, tileHeight * 16);
            m_enemies[i] += offset * 16;
            m_enemies[i].shrink(20);
        }

        m_guffins = detectLines(bg, HORIZONTAL, TILE_MAC_GUFFIN);
        for (int i = 0; i < m_guffins.size(); ++i)
        {
            m_guffins[i].scale(tileWidth * 16, tileHeight * 16);
            m_guffins[i] += offset * 16;
        }

        for (int x = 0; x < bg.width(); ++x)
        {
            for (int y = 0; y < bg.height(); ++y)
            {
                if (bg.get(x,y) == 2)
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
