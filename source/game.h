#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include "shared_ptr.h"
#include "physics.h"
#include "physics_event.h"
#include "tiny_string.h"
#include "actor_animation_controller.h"
#include "enemy.h"
#include "fire_ball.h"
#include "sound_controller.h"

// forward declarations
class VgaGfx;
class ImageBase;
class Animation;
class CompiledSprite;
class SeekerEnemy;

namespace ActorPosition
{
	enum ActorPositionT
	{
		UseSpawnPoint,
		LevelTransition,
		KeepActorPos
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
	shared_ptr<Animation> seekerEnemyAnimation;
	shared_ptr<Animation> guffinAnimation;
	shared_ptr<Animation> fireBallAnimation;
	shared_ptr<Animation> jetPackAnimation;
};


struct CollectedGuffin
{
	LevelNumber level;
	Point pos;
};


class Game : public PhysicsCallback
{
public:
	Game(
	    shared_ptr<VgaGfx> vgaGfx,
		shared_ptr<SoundController> sound, 
		shared_ptr<ImageBase> tiles,
		GameAnimations animations,
		const char* levelBasename,
		LevelNumber startLevel);

	void reloadCurrentLevel();

	void loadLevel(LevelNumber levelNumber, ActorPosition::ActorPositionT actorPosition);
    void drawFrame();

	// PhysicsCallback interface
	virtual void levelTransition(LevelTransition transition);
	virtual void collectApple(Point point);
	virtual void collectJetPack(Point point);
	virtual void touchButton(uint16_t id, ButtonType type);
	virtual void onDeath();

private:
	virtual void drawAppleCount();
    virtual void drawDeathCount();

private:
	shared_ptr<VgaGfx> m_vgaGfx;
	shared_ptr<ImageBase> m_tiles;
	GameAnimations m_animations;
	tnd::vector<shared_ptr<Enemy> > m_enemies;
	tnd::vector<shared_ptr<SeekerEnemy> > m_seekerEnemies;
	tnd::vector<shared_ptr<FireBall> > m_fireBalls;
	tnd::vector<Rectangle> m_guffins;
	tnd::vector<Rectangle> m_jetPacks;
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
	TinyString m_deathString;
	shared_ptr<SoundController> m_sound;
	uint8_t m_jetpackCollected; // 0 == no jetpack, 1 == jetpack collected
	uint8_t m_button1; // 0 == button not pressed, 1 == button pressed
	uint32_t m_deathCounter;
	bool m_levelMustReload;
};


#endif