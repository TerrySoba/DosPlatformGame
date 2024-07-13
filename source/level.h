#ifndef LEVEL_H_INCLUDED
#define LEVEL_H_INCLUDED

#include "image_base.h"
#include "drawable.h"
#include "button.h"
#include "rectangle.h"
#include "vector.h"
#include "shared_ptr.h"
#include "tiny_string.h"

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
    Level(const char* mapFilename,
          int16_t tileWidth, int16_t tileHeight,
          int16_t offsetX, int16_t offsetY);

    void setTilesImage(tnd::shared_ptr<ImageBase> tilesImage);

    virtual ~Level();

    // Interface Drawable
    virtual int16_t width() const;
    virtual int16_t height() const;
    virtual void draw(char* target, int16_t targetWidth, int16_t targetHeight, int16_t targetX, int16_t targetY) const;

    tnd::vector<Rectangle> getWalls() { return m_walls; }
    tnd::vector<Rectangle> getGhostWalls() { return m_ghostWalls; }
    tnd::vector<Rectangle> getDeath() { return m_death; }
    tnd::vector<Rectangle> getFallThrough() { return m_fallThrough; }
    tnd::vector<Rectangle> getEnemies() { return m_enemies; }
    tnd::vector<Rectangle> getSeekerEnemies() { return m_seekerEnemies; }
    tnd::vector<Rectangle> getMacGuffins() { return m_guffins; }
    tnd::vector<Rectangle> getJetPacks() { return m_jetPacks; }
    tnd::vector<Rectangle> getSunItems() { return m_sunItems; }
    tnd::vector<MessageBox> getMessageBoxes() { return m_messageBoxes; }
    tnd::vector<Rectangle> getFireBalls() { return m_fireBalls; }
    tnd::vector<Rectangle> getBoss1() { return m_boss1; }
    tnd::vector<Rectangle> getBoss2() { return m_boss2; }
    tnd::vector<Rectangle> getPlayTime() { return m_playTime; }
    tnd::vector<Rectangle> getTentacles() { return m_tentacles; }
    tnd::vector<Rectangle> getTentacleArms() { return m_tentacleArms; }
    tnd::vector<Button> getButtons() { return m_buttons; }
    tnd::vector<Rectangle> getEyes() { return m_eyes; }
    uint16_t getMusicIndex() { return m_musicIndex; }
    uint16_t getCutscene() { return m_cutscene; }


    Point getSpawnPoint() { return m_spawn; };

    Point getSunPoint() { return m_sun; };

    tnd::vector<uint8_t>& getMapData() { return m_mapData; }

    uint16_t getGuffinGate() { return m_guffinGate; }

    TinyString getTileset() { return m_tileset; }

private:
    tnd::shared_ptr<ImageBase> m_tilesImage;
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
    tnd::vector<Rectangle> m_jetPacks;
    tnd::vector<Rectangle> m_sunItems;
    tnd::vector<MessageBox> m_messageBoxes;
    tnd::vector<Rectangle> m_fireBalls;
    tnd::vector<Rectangle> m_seekerEnemies;
    tnd::vector<Rectangle> m_boss1;
    tnd::vector<Rectangle> m_boss2;
    tnd::vector<Rectangle> m_playTime;
    tnd::vector<Rectangle> m_tentacles;
    tnd::vector<Rectangle> m_tentacleArms;
    tnd::vector<Button> m_buttons;
    tnd::vector<Rectangle> m_eyes;

    TinyString m_tileset;

    Point m_spawn;
    Point m_sun;
    uint16_t m_guffinGate;
    uint16_t m_musicIndex;
    uint16_t m_cutscene;
};

#endif
