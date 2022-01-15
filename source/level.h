#ifndef LEVEL_H_INCLUDED
#define LEVEL_H_INCLUDED

#include "image_base.h"
#include "drawable.h"
#include "shared_ptr.h"
#include "rectangle.h"
#include "vector.h"

struct MessageBox
{
    MessageBox() :
        textId(0), x(0), y(0), w(0), h(0)
    {}

    MessageBox(uint16_t _textId, uint16_t _x, uint16_t _y, uint16_t _w, uint16_t _h) :
        textId(_textId), x(_x), y(_y), w(_w), h(_h)
    {}

    uint16_t textId;
    uint16_t x;
    uint16_t y;
    uint16_t w;
    uint16_t h;
};


class Level : public Drawable
{
public:
    Level(const char* mapFilename, shared_ptr<ImageBase> tilesImage,
          int16_t tileWidth, int16_t tileHeight,
          int16_t offsetX, int16_t offsetY);

    virtual ~Level();

    virtual int16_t width() const;
    virtual int16_t height() const;
    virtual void draw(char* target, int16_t targetWidth, int16_t targetHeight, int16_t targetX, int16_t targetY) const;

    virtual tnd::vector<Rectangle> getWalls() { return m_walls; }
    virtual tnd::vector<Rectangle> getGhostWalls() { return m_ghostWalls; }
    virtual tnd::vector<Rectangle> getDeath() { return m_death; }
    virtual tnd::vector<Rectangle> getFallThrough() { return m_fallThrough; }
    virtual tnd::vector<Rectangle> getEnemies() { return m_enemies; }
    virtual tnd::vector<Rectangle> getMacGuffins() { return m_guffins; }
    virtual tnd::vector<MessageBox> getMessageBoxes() { return m_messageBoxes; }
    virtual tnd::vector<Rectangle> getFireBalls() { return m_fireBalls; }


    virtual Point getSpawnPoint() { return m_spawn; };


    virtual tnd::vector<uint8_t>& getMapData() { return m_mapData; }

private:
    shared_ptr<ImageBase> m_tilesImage;
    int16_t m_tileWidth;  // width of a single tile in pixles
    int16_t m_tileHeight; // height of a single tile in pixles
    int16_t m_offsetX;
    int16_t m_offsetY;

    int16_t m_mapWidth;   // number of tiles in horizontal direction
    int16_t m_mapHeight;  // number of tiles in vertical direction
    tnd::vector<uint8_t> m_mapData;

    tnd::vector<Rectangle> m_walls;
    tnd::vector<Rectangle> m_ghostWalls;
    tnd::vector<Rectangle> m_death;
    tnd::vector<Rectangle> m_fallThrough;
    tnd::vector<Rectangle> m_enemies;
    tnd::vector<Rectangle> m_guffins;
    tnd::vector<MessageBox> m_messageBoxes;
    tnd::vector<Rectangle> m_fireBalls;
    Point m_spawn;
};

#endif
