#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include "shared_ptr.h"
#include "physics.h"
#include "physics_event.h"
#include "tiny_string.h"
#include "actor_animation_controller.h"
#include "enemy.h"

// forward declarations
class VgaGfx;
class ImageBase;
class Animation;
class CompiledSprite;

namespace UseSpawnPoint
{
	enum UseSpawnPointT
	{
		NO,
		YES
	};
}

struct LevelNumber
{
	int8_t x;
	int8_t y;

	bool operator==(const LevelNumber& other)
	{
		return (x == other.x && y == other.y);
	}
};


struct GameAnimations
{
	shared_ptr<Animation> actorAnimation;
	shared_ptr<Animation> enemyAnimation;
	shared_ptr<Animation> guffinAnimation;
};


struct CollectedGuffin
{
	LevelNumber level;
	Point pos;
};


class Game : public PhysicsCallback
{
public:
	Game(shared_ptr<VgaGfx> vgaGfx, shared_ptr<ImageBase> tiles, GameAnimations animations, const char* levelBasename, LevelNumber startLevel);

	void loadLevel(LevelNumber levelNumber, UseSpawnPoint::UseSpawnPointT useSpawnPoint);
    void drawFrame();

	// PhysicsCallback interface
	virtual void levelTransition(LevelTransition transition);
	virtual void collectApple(Point point);

private:
	virtual void drawAppleCount();

private:
	shared_ptr<VgaGfx> m_vgaGfx;
	shared_ptr<ImageBase> m_tiles;
	GameAnimations m_animations;
	tnd::vector<shared_ptr<Enemy> > m_enemies;
	tnd::vector<Rectangle> m_guffins;
    shared_ptr<Physics> m_physics;
    long int m_frames;
    int m_player;
	TinyString m_levelBasename;
	LevelNumber m_levelNumber;
	LevelNumber m_nextLevel;
	ActorAnimationController m_animationController;
	bool m_lastButtonPressed;
	tnd::vector<CollectedGuffin> m_collectedGuffins;
	TinyString m_appleString;
};


#endif