#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include "physics.h"
#include "physics_event.h"
#include "tiny_string.h"
#include "actor_animation_controller.h"
#include "sound_controller.h"
#include "music_controller.h"
#include "ptr_vector.h"
#include "animation.h"
#include "gfx_output.h"
#include "enemy.h"
#include "tentacle.h"
#include "tentacle_arm.h"
#include "seeker_enemy.h"
#include "fire_ball.h"
#include "rectangle.h"
#include "boss1.h"
#include "boss2.h"
#include "shared_ptr.h"
#include "key_mapper.h"
#include "eye.h"


enum StoryStatus
{
    STORY_STATUS_INITIAL = 0,
    STORY_STATUS_INTRO_SHOWN = 1,
};


namespace ActorPosition
{
	enum ActorPositionT
	{
		UseSpawnPoint,
		LevelTransition,
		KeepActorPos,
		LoadSaveGame,
	};
}

struct LevelNumber
{
	LevelNumber() : x(0), y(0) {}
	LevelNumber(int8_t x, int8_t y) : x(x), y(y) {}

	int8_t x;
	int8_t y;

	bool operator==(const LevelNumber& other) const
	{
		return (x == other.x && y == other.y);
	}
};


struct GameAnimations
{
	tnd::shared_ptr<Animation> actorAnimation;
	tnd::shared_ptr<Animation> enemyAnimation;
	tnd::shared_ptr<Animation> seekerEnemyAnimation;
	tnd::shared_ptr<Animation> guffinAnimation;
	tnd::shared_ptr<Animation> fireBallAnimation;
	tnd::shared_ptr<Animation> jetPackAnimation;
	tnd::shared_ptr<Animation> tentacleAnimation;
	tnd::shared_ptr<Animation> projectileAnimation;
	tnd::shared_ptr<Animation> tentacleArmAnimation;
	tnd::shared_ptr<Animation> eyeAnimation;
};


struct CollectedGuffin
{
	LevelNumber level;
	Point pos;
};

enum GameExitCode
{
	GAME_EXIT_QUIT,
	GAME_EXIT_CREDITS
};


class Game : public PhysicsCallback
{
public:
	Game(
	    tnd::shared_ptr<GfxOutput> vgaGfx,
		tnd::shared_ptr<SoundController> sound, 
		tnd::shared_ptr<MusicController> music,
		GameAnimations animations,
		const char* levelBasename,
		LevelNumber startLevel);

	~Game();

	void reloadCurrentLevel();

	void loadLevel(LevelNumber levelNumber, ActorPosition::ActorPositionT actorPosition);
    void drawFrame();

	GameExitCode runGameLoop();

	uint32_t getFrameCount() const { return m_frameCounter; }

	// PhysicsCallback interface
	virtual void levelTransition(LevelTransition transition);
	virtual void collectApple(Point point);
	virtual void collectJetPack(Point point);
	virtual void collectSunItem(Point point);
	virtual void touchButton(uint16_t id, ButtonType type);
	virtual void onDeath();
	virtual void onCreditsWarp();

private:
	virtual void drawAppleCount();
    virtual void drawDeathCount();

private:
	tnd::shared_ptr<GfxOutput> m_vgaGfx;
	tnd::shared_ptr<ImageBase> m_tiles;
	GameAnimations m_animations;
	tnd::ptr_vector<Enemy> m_enemies;
	tnd::ptr_vector<SeekerEnemy> m_seekerEnemies;
	tnd::ptr_vector<FireBall> m_fireBalls;
	tnd::ptr_vector<Boss1> m_boss1;
	tnd::ptr_vector<Boss2> m_boss2;
	tnd::ptr_vector<Tentacle> m_tentacles;
	tnd::ptr_vector<TentacleArm> m_tentacleArms;
	tnd::ptr_vector<Eye> m_eyes;
	tnd::vector<Rectangle> m_guffins;
	tnd::vector<Rectangle> m_jetPacks;
	tnd::vector<Rectangle> m_sunItems;
    tnd::shared_ptr<Physics> m_physics;
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
	tnd::shared_ptr<SoundController> m_sound;
	tnd::shared_ptr<MusicController> m_music;
	uint8_t m_jetpackCollected; // 0 == no jetpack, 1 == jetpack collected
	uint8_t m_sunItemCollected; // 0 == not collected, 1 == collected
	uint8_t m_button1; // 0 == button not pressed, 1 == button pressed
	uint32_t m_deathCounter;
	bool m_levelMustReload;
	uint32_t m_frameCounter;
	KeyMapper m_keyMapper;
	TinyString m_loadedTilesetName;
	StoryStatus m_storyStatus;
	GameExitCode m_exitCode;
	uint32_t m_deathFramesLeft;
};


#endif