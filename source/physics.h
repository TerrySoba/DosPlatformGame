#ifndef PHYSICS_H_INCLUDED
#define PHYSICS_H_INCLUDED

#include "rectangle.h"
#include "physics_event.h"
#include "sound_controller.h"

#include "shared_ptr.h"
#include "vector.h"
#include <stdint.h>

// Physics calculations are done on subpixels.
// One pixel constitutes 16 subpixels, so shifting by 4
// is used to transform between pixels and subpixels.

#define PIXEL_TO_SUBPIXEL(x) ((x) << 4)
#define SUBPIXEL_TO_PIXEL(x) ((x) >> 4)

struct Actor
{
    Actor() :
        dx(0), dy(0), isOnGround(false),
        isDucking(false), isFallingThrough(false),
        jetpackFrames(0), jetpackLockoutFrames(0)
    {
    }


    Rectangle rect;
    int16_t dx, dy;
    bool isOnGround;
    bool isDucking;
    bool isFallingThrough;
    uint16_t jetpackFrames; // frames that the jetpack may be activated
    uint16_t jetpackLockoutFrames; // lock out jetpack for a ceratain amount of frames after jump
};

enum IntersectionType
{
    INTERSECTION_TOP = 1,
    INTERSECTION_BOTTOM = 2,
    INTERSECTION_LEFT = 3,
    INTERSECTION_RIGHT = 4,
    INTERSECTION_OTHER = 5
};


class Physics
{
public:
    Physics(PhysicsCallback* callback, shared_ptr<SoundController> sound);
    int addActor(const Actor& rect);
    void setActor(int index, const Actor& rect);
    void getActorPos(int index, int16_t& x, int16_t& y);
    void setActorSpeedX(int index, int16_t dx);
    void setActorSpeedY(int index, int16_t dy);
    void setActorDuck(int index, bool isDucking);
    void startActorJump(int index);
    void stopActorJump(int index);
    void activateJetpack(int index);

    void setWalls(const tnd::vector<Rectangle>& walls);
    void setDeath(const tnd::vector<Rectangle>& death);
    void setEnemyDeath(const tnd::vector<Rectangle>& death);
    void setFallThrough(const tnd::vector<Rectangle>& fallThrough);
    void setGuffins(const tnd::vector<Rectangle>& guffins);
    void setSpawnPoint(const Point& point);


    void calc();

    static bool intersectRect(const Rectangle &r1, const Rectangle &r2);

    /**
     * Get type of intersection between r1 and r2.
     * r1 is the reference object so if the right side of r2 intersects
     * with the left side of r1 you get INTERSECTION_LEFT.
     */
    static IntersectionType getIntersectionType(const Rectangle &r1, const Rectangle &r2);

public:
    tnd::vector<Rectangle> m_walls;
    tnd::vector<Rectangle> m_death;
    tnd::vector<Rectangle> m_enemyDeath;
    tnd::vector<Rectangle> m_fallThrough;
    tnd::vector<Rectangle> m_guffins;
    Rectangle m_rightLevelTransition;
    Rectangle m_leftLevelTransition;
    Rectangle m_bottomLevelTransition;
    Rectangle m_topLevelTransition;
    Point m_spawn;
    tnd::vector<Actor> m_actors;
    PhysicsCallback* m_callback;
    shared_ptr<SoundController> m_sound;
};

#endif
