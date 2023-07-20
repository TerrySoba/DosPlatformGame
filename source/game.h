#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include "physics.h"
#include "physics_event.h"
#include "tiny_string.h"
#include "actor_animation_controller.h"
#include "enemy.h"
#include "fire_ball.h"
#include "sound_controller.h"
#include "boss1.h"
#include "music_controller.h"
#include "shared_ptr.h"

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
	Animation* actorAnimation;
	Animation* enemyAnimation;
	Animation* seekerEnemyAnimation;
	Animation* guffinAnimation;
	Animation* fireBallAnimation;
	Animation* jetPackAnimation;
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
	    VgaGfx* vgaGfx,
		SoundController* sound, 
		MusicController* music,
		ImageBase* tiles,
		GameAnimations animations,
		const char* levelBasename,
		LevelNumber startLevel);

	~Game();

	void reloadCurrentLevel();

	void loadLevel(LevelNumber levelNumber, ActorPosition::ActorPositionT actorPosition);
    void drawFrame();

	uint32_t getFrameCount() const { return m_frameCounter; }

	// PhysicsCallback interface
	virtual void levelTransition(LevelTransition transition);
	virtual void collectApple(Point point);
	virtual void collectJetPack(Point point);
	virtual void collectSunItem(Point point);
	virtual void touchButton(uint16_t id, ButtonType type);
	virtual void onDeath();

private:
	virtual void drawAppleCount();
    virtual void drawDeathCount();

private:
	VgaGfx* m_vgaGfx;
	ImageBase* m_tiles;
	GameAnimations m_animations;
	tnd::vector<shared_ptr<Enemy> > m_enemies;
	tnd::vector<shared_ptr<SeekerEnemy> > m_seekerEnemies;
	tnd::vector<shared_ptr<FireBall> > m_fireBalls;
	tnd::vector<shared_ptr<Boss1> > m_boss1;
	tnd::vector<Rectangle> m_guffins;
	tnd::vector<Rectangle> m_jetPacks;
	tnd::vector<Rectangle> m_sunItems;
    Physics* m_physics;
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
	SoundController* m_sound;
	MusicController* m_music;
	uint8_t m_jetpackCollected; // 0 == no jetpack, 1 == jetpack collected
	uint8_t m_sunItemCollected; // 0 == not collected, 1 == collected
	uint8_t m_button1; // 0 == button not pressed, 1 == button pressed
	uint32_t m_deathCounter;
	bool m_levelMustReload;
	uint32_t m_frameCounter;
};


#endif