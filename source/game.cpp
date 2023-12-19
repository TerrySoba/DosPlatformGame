#include "game.h"
#include "level.h"
#include "image_base.h"
#include "physics.h"
#include "version.h"
#include "vector.h"
#include "exception.h"
#include "compiled_sprite.h"
#include "game_save.h"
#include "text.h"
#include "i18n.h"
#include "tile_definitions.h"
#include "time_tools.h"

#include <stdio.h>

Game::Game(tnd::shared_ptr<VgaGfx> vgaGfx, tnd::shared_ptr<SoundController> sound,
           tnd::shared_ptr<MusicController> music,
           GameAnimations animations,
           const char* levelBasename, LevelNumber startLevel) :
    m_vgaGfx(vgaGfx), m_animations(animations), m_frames(0), m_levelBasename(levelBasename),
    m_animationController(animations.actorAnimation, sound), m_lastButtonPressed(false), m_sound(sound), m_music(music),
    m_jetpackCollected(1), m_sunItemCollected(1), m_button1(1), m_levelMustReload(false), m_deathCounter(0),
    m_frameCounter(0)
{

    m_nextLevel.x = -1;
    m_nextLevel.y = -1;

    m_appleString = I18N::getString(2);
    m_deathString = I18N::getString(26);

    // try to load savegame
    GameState state;
    if (loadGameState(state, "game.sav"))
    {
        m_collectedGuffins = state.colectedGuffins;
        m_jetpackCollected = state.jetpackCollected;
        m_button1 = state.button1;
        m_deathCounter = state.deathCounter;
        m_frameCounter = state.frameCounter;

        loadLevel(state.level, ActorPosition::LoadSaveGame);   

        m_physics->setSpawnPoint(state.spawnPoint);
        drawAppleCount();
        drawDeathCount();
    }
    else
    {
        loadLevel(startLevel, ActorPosition::UseSpawnPoint);
    }
}

Game::~Game()
{
}

void Game::reloadCurrentLevel()
{
    loadLevel(m_levelNumber, ActorPosition::KeepActorPos);
}


bool fileExists(const char* fileName)
{
    FILE *file;
    if ((file = fopen(fileName, "rb")))
    {
        fclose(file);
        return true;
    }
    return false;
}


void Game::loadLevel(LevelNumber levelNumber, ActorPosition::ActorPositionT actorPosition)
{
    LevelNumber previousLevel = m_levelNumber;
    m_levelNumber = levelNumber;
    char buf[16];

    intToString(m_levelNumber.x, 16, buf, 16, 2, '0');
    intToString(m_levelNumber.y, 16, buf + 2, 14, 2, '0');

    TinyString levelMap = TinyString(buf + TinyString(".map"));

    if (m_button1 == 1)
    {
        TinyString buttonLevelName = TinyString(buf + TinyString("b1.map"));
        if (fileExists(buttonLevelName.c_str()))
        {
            levelMap = buttonLevelName;
        }
    }

    Level level(levelMap.c_str(), 16, 16, -8, -8);

    // load new tileset if it has changed
    if (m_loadedTilesetName != level.getTileset())
    {
        m_loadedTilesetName = level.getTileset();
        m_tiles.reset();
        m_tiles = new TgaImage(m_loadedTilesetName.c_str());
    }

    level.setTilesImage(m_tiles);


    m_music->playMusic((SongIndex)level.getMusicIndex());

    m_animations.actorAnimation->useTag("LoopR");


    int16_t actorPosX, actorPosY;
    if (actorPosition == ActorPosition::KeepActorPos ||
        actorPosition == ActorPosition::LoadSaveGame)
    {
        m_physics->getActorPos(m_player, actorPosX, actorPosY);
    }
    else if (m_physics.get() == 0 || actorPosition == ActorPosition::UseSpawnPoint)
    {
        // so no previous position existed, or use of spawn point was explicitly requested
        // because of that we just put the actor to the defined spawn point of the level
        actorPosX = PIXEL_TO_SUBPIXEL(level.getSpawnPoint().x);
        actorPosY = PIXEL_TO_SUBPIXEL(level.getSpawnPoint().y);
    }
    else
    {
        // we use the y position of the actor from the previous level
        m_physics->getActorPos(m_player, actorPosX, actorPosY);
        if (previousLevel.x < levelNumber.x) // left to right
        {
            actorPosX = PIXEL_TO_SUBPIXEL(10);
        }
        else if (previousLevel.x > levelNumber.x) // right to left
        {
            actorPosX = PIXEL_TO_SUBPIXEL(310 - m_animations.actorAnimation->width());
        }
        else if (previousLevel.y < levelNumber.y) // top to bottom
        {
            actorPosY = PIXEL_TO_SUBPIXEL(10);
        }
        else if (previousLevel.y > levelNumber.y) // bottom to top
        {
            actorPosY = PIXEL_TO_SUBPIXEL(185 - m_animations.actorAnimation->height());
        }
    }

    // now load enemies
    {
        m_enemies.clear();
        tnd::vector<Rectangle> enemyRectangles = level.getEnemies();
        for (int i = 0; i < enemyRectangles.size(); ++i)
        {
            m_enemies.push_back(new Enemy(enemyRectangles[i], m_animations.enemyAnimation));
        }
    }

    // now load tentacles
    {
        m_tentacles.clear();
        tnd::vector<Rectangle> tentacleRectangles = level.getTentacles();
        for (int i = 0; i < tentacleRectangles.size(); ++i)
        {
            m_tentacles.push_back(new Tentacle(tentacleRectangles[i].x, tentacleRectangles[i].y, m_animations.tentacleAnimation));
        }
    }

    // now load tentacleArms
    {
        m_tentacleArms.clear();
        tnd::vector<Rectangle> tentacleArmRectangles = level.getTentacleArms();
        for (int i = 0; i < tentacleArmRectangles.size(); ++i)
        {
            m_tentacleArms.push_back(new TentacleArm(tentacleArmRectangles[i].x, tentacleArmRectangles[i].y, m_animations.tentacleAnimation));
        }
    }

    // now load seeker enemies
    {
        m_seekerEnemies.clear();
        tnd::vector<Rectangle> seekerEnemyRectangles = level.getSeekerEnemies();
        for (int i = 0; i < seekerEnemyRectangles.size(); ++i)
        {
            m_seekerEnemies.push_back(new SeekerEnemy(seekerEnemyRectangles[i], m_animations.seekerEnemyAnimation));
        }
    }

    // now load fireballs if any exist
    {
        m_fireBalls.clear();
        tnd::vector<Rectangle> fireBalls = level.getFireBalls();
        for (int i = 0; i < fireBalls.size(); ++i)
        {
            m_fireBalls.push_back(new FireBall(fireBalls[i], m_animations.fireBallAnimation));
        }
    }

    m_guffins.clear();
    m_guffins = level.getMacGuffins();

    m_jetPacks.clear();
    if (m_jetpackCollected == 0) // only load jetpack elements if it has not been collected yet
    {
        m_jetPacks = level.getJetPacks();
    }

    m_sunItems.clear();
    if (m_sunItemCollected == 0) // only load sunItem elements if it has not been collected yet
    {
        m_sunItems = level.getSunItems();
    }

    // remove already collected guffins
    for (int n = 0; n < m_collectedGuffins.size(); ++n)
    {
        CollectedGuffin collected = m_collectedGuffins[n];
        int index = -1;
        for (int i = 0; i < m_guffins.size(); ++i)
        {
            Rectangle& rect = m_guffins[i];
            if (m_levelNumber == collected.level && 
                rect.x == collected.pos.x &&
                rect.y == collected.pos.y)
            {
                index = i;
            }      
        }

        if (index >= 0) m_guffins.erase(index);
    }

    // now load boss1 if any exist
    {
        m_boss1.clear();
        tnd::vector<Rectangle> boss1Rects = level.getBoss1();
        
        // create boss1 instances
        for (int i = 0; i < boss1Rects.size(); ++i)
        {
            m_boss1.push_back(new Boss1(boss1Rects[i], m_animations.fireBallAnimation, level.getWalls()));
        }
    }

    // now load boss2 if any exist
    {
        // m_boss1.clear();
        tnd::vector<Rectangle> boss2Rects = level.getBoss2();
        
        // create boss1 instances
        for (int i = 0; i < boss2Rects.size(); ++i)
        {
            m_boss1.push_back(new Boss1(boss2Rects[i], m_animations.fireBallAnimation, level.getWalls()));
        }
    }

    m_physics.reset();
    m_physics = tnd::shared_ptr<Physics>(new Physics(this, m_sound));
    Actor actor;
    actor.rect.x = actorPosX;
    actor.rect.y = actorPosY;
    actor.rect.width = PIXEL_TO_SUBPIXEL(m_animations.actorAnimation->width());
    actor.rect.height = PIXEL_TO_SUBPIXEL(m_animations.actorAnimation->height());
    actor.dx = 0;
    actor.dy = 0;
    m_player = m_physics->addActor(actor);


    tnd::vector<Rectangle> walls = level.getWalls();
    tnd::vector<Rectangle> ghostWalls = level.getGhostWalls();

    // add ghost blocks to walls if enough guffins have been collected
    if (m_collectedGuffins.size() >= level.getGuffinGate())
    {
        // add ghost blocks as normal walls
        for (int i = 0; i < ghostWalls.size(); ++i)
        {
            walls.push_back(ghostWalls[i]);
        }

        tnd::vector<uint8_t>& mapData = level.getMapData();

        // increment the visible tiles gfx of the ghost blocks by one
        // this makes the transparent blocks into solid blocks
        for (int i = 0; i < mapData.size(); ++i)
        {
            if (mapData[i] == GFX_TILE_GHOST_GROUND_1 || mapData[i] == GFX_TILE_GHOST_GROUND_2) {
                ++mapData[i];
            }
        }
    }

    if (m_button1 == 1)
    {
        tnd::vector<uint8_t>& mapData = level.getMapData();

        // increment the visible tiles gfx of the button1 blocks by one
        // this makes the button gfx show on
        for (int i = 0; i < mapData.size(); ++i)
        {
            if (mapData[i] == GFX_TILE_SWITCH_1_ON) {
                ++mapData[i];
            }
            if (mapData[i] == GFX_TILE_SWITCH_1_OFF) {
                ++mapData[i];
            }
        }
    }

    m_physics->setWalls(walls);
    m_physics->setDeath(level.getDeath());
    m_physics->setFallThrough(level.getFallThrough());
    m_physics->setButtons(level.getButtons());
    m_physics->setGuffins(m_guffins);
    m_physics->setJetPacks(m_jetPacks);
    m_physics->setSunItems(m_sunItems);

    
    m_physics->setSpawnPoint(Point(actorPosX, actorPosY));
    m_physics->setSunPoint(level.getSunPoint());
    m_vgaGfx->clear();

    


    m_vgaGfx->drawBackground(level, -8, -8);



    tnd::vector<MessageBox> messageBoxes = level.getMessageBoxes();
    for (int i = 0; i < messageBoxes.size(); ++i) {
        MessageBox& messageBox = messageBoxes[i];
        TinyString message = I18N::getString(messageBox.textId);
        Text t(message.c_str(), messageBox.w / 5, true);
        m_vgaGfx->drawBackground(t, messageBox.x, messageBox.y);
    }

    tnd::vector<Rectangle> playTimes = level.getPlayTime();
    for (int i = 0; i < playTimes.size(); ++i) {
        Rectangle& playTime = playTimes[i];
        Text t(createTimeString(m_frameCounter), 100, true);
        m_vgaGfx->drawBackground(t, playTime.x, playTime.y);
    }

    TinyString levelString = I18N::getString((m_levelNumber.y << 6) + m_levelNumber.x);
    Text t(levelString.c_str(), 0, false);
    m_vgaGfx->drawBackground(t, 50, 193);
    drawAppleCount();
    drawDeathCount();

    // auto save current state
    GameState state;
    state.level = m_levelNumber;
    state.spawnPoint = Point(actorPosX, actorPosY);
    state.colectedGuffins = m_collectedGuffins;
    state.jetpackCollected = m_jetpackCollected;
    state.button1 = m_button1;
    state.deathCounter = m_deathCounter;
    state.frameCounter = m_frameCounter;
    
    if (actorPosition != ActorPosition::LoadSaveGame)
    {
        saveGameState(state, "game.sav");
    }
}

void Game::drawAppleCount()
{
    char buf[16];
    intToString(m_collectedGuffins.size(), 10, buf, 16, 3);
    TinyString appleString = m_appleString + buf;
    Text t(appleString.c_str());
    m_vgaGfx->drawBackground(t, 260, 1);
}

void Game::drawDeathCount()
{
    char buf[16];
    intToString(m_deathCounter, 10, buf, 16);
    TinyString deathString = m_deathString + buf;
    Text t(deathString.c_str());
    m_vgaGfx->drawBackground(t, 10, 1);
}

void Game::collectApple(Point point)
{
    CollectedGuffin collected;
    collected.level = m_levelNumber;
    collected.pos = point;
    m_collectedGuffins.push_back(collected);
    drawAppleCount();

    // remove colected apple from guffin list
    int index = -1;
    for (int i = 0; i < m_guffins.size(); ++i)
    {
        Rectangle& guffin = m_guffins[i];
        if (guffin.x == point.x && guffin.y == point.y)
        {
            index = i;
        }
    }

    if (index >= 0) m_guffins.erase(index);
    m_physics->setGuffins(m_guffins);
    m_sound->playGuffinSound();
}

void Game::collectJetPack(Point point)
{
    // set jetpack collected flag in game state
    m_jetpackCollected = 1;

    // remove colected jetPack from jetPack list
    int index = -1;
    for (int i = 0; i < m_jetPacks.size(); ++i)
    {
        Rectangle& jetPack = m_jetPacks[i];
        if (jetPack.x == point.x && jetPack.y == point.y)
        {
            index = i;
        }
    }

    if (index >= 0) m_jetPacks.erase(index);
    m_physics->setJetPacks(m_jetPacks);
    m_sound->playJetpackSound();
}


void Game::collectSunItem(Point point)
{
    // set sunItem collected flag in game state
    m_sunItemCollected = 1;

    // remove colected sunItem from sunItem list
    int index = -1;
    for (int i = 0; i < m_sunItems.size(); ++i)
    {
        Rectangle& sunItem = m_sunItems[i];
        if (sunItem.x == point.x && sunItem.y == point.y)
        {
            index = i;
        }
    }

    if (index >= 0) m_sunItems.erase(index);
    m_physics->setSunItems(m_sunItems);
    m_sound->playJetpackSound();
}


void Game::touchButton(uint16_t id, ButtonType type)
{
    if (id == 1 && type == BUTTON_ON && m_button1 == 0)
    {
        m_button1 = 1;
        m_sound->playSwitchSound();
        m_levelMustReload = true; // mark level to be reloaded in text frame
    }

    if (id == 1 && type == BUTTON_OFF && m_button1 == 1)
    {
        m_button1 = 0;
        m_sound->playSwitchSound();
        m_levelMustReload = true; // mark level to be reloaded in text frame
    }
}

void Game::onDeath()
{
    ++m_deathCounter;
    drawDeathCount();
    m_vgaGfx->drawDeathEffect();
}

void Game::drawFrame()
{
    ++m_frameCounter;
    if (m_nextLevel.x != -1)
    {
        loadLevel(m_nextLevel, ActorPosition::LevelTransition);
        m_nextLevel.x = -1;
    }

    if (m_levelMustReload)
    {
        m_levelMustReload = false;
        reloadCurrentLevel();
    }

    m_vgaGfx->clear();

    int16_t playerX;
    int16_t playerY;
    m_physics->getActorPos(m_player, playerX, playerY);

    m_animationController.setPos(playerX, playerY, m_physics->jetpackIsActive());
    
    tnd::vector<Rectangle> enemyDeath;

    for (int i = 0; i < m_enemies.size(); ++i)
    {
        Enemy* enemyPtr = m_enemies[i];
        enemyPtr->walk();
        Rectangle enemy = enemyPtr->getPos();
        enemyDeath.push_back(enemy);
        m_vgaGfx->draw(*m_animations.enemyAnimation, SUBPIXEL_TO_PIXEL(enemy.x), SUBPIXEL_TO_PIXEL(enemy.y));
    }

    for (int i = 0; i < m_tentacles.size(); ++i)
    {
        Tentacle* tentaclePtr = m_tentacles[i];
        tentaclePtr->act();
        Rectangle tentacle = tentaclePtr->getPos();
        m_vgaGfx->draw(*m_animations.tentacleAnimation, SUBPIXEL_TO_PIXEL(tentacle.x), SUBPIXEL_TO_PIXEL(tentacle.y));
        tentacle.shrink(32);
        enemyDeath.push_back(tentacle);


        tnd::vector<Rectangle> projectiles = tentaclePtr->getProjectiles();

        // draw projectiles
        for (int j = 0; j < projectiles.size(); ++j)
        {
            Rectangle& projectile = projectiles[j];
            enemyDeath.push_back(projectile);
            m_vgaGfx->draw(*m_animations.projectileAnimation, SUBPIXEL_TO_PIXEL(projectile.x), SUBPIXEL_TO_PIXEL(projectile.y));
        }

    }

    for (int i = 0; i < m_tentacleArms.size(); ++i)
    {
        TentacleArm* tentacleArmPtr = m_tentacleArms[i];
        tentacleArmPtr->act(playerX, playerY);
      
        tnd::vector<Rectangle> tentacleArmSegments = tentacleArmPtr->getSegments();

        // draw projectiles
        for (int j = 0; j < tentacleArmSegments.size(); ++j)
        {
            Rectangle& segment = tentacleArmSegments[j];
            enemyDeath.push_back(segment);
            m_vgaGfx->draw(*m_animations.tentacleArmAnimation, SUBPIXEL_TO_PIXEL(segment.x), SUBPIXEL_TO_PIXEL(segment.y));
        }

    }

    for (int i = 0; i < m_seekerEnemies.size(); ++i)
    {
        SeekerEnemy* enemyPtr = m_seekerEnemies[i];
        enemyPtr->walk(Rectangle(playerX, playerY, 1, 1));
        Rectangle enemy = enemyPtr->getPos();
        enemyDeath.push_back(enemy);
        m_vgaGfx->draw(*m_animations.seekerEnemyAnimation, SUBPIXEL_TO_PIXEL(enemy.x), SUBPIXEL_TO_PIXEL(enemy.y));
    }

    for (int i = 0; i < m_fireBalls.size(); ++i)
    {
        FireBall* enemyPtr = m_fireBalls[i];
        enemyPtr->walk();
        Rectangle enemy = enemyPtr->getPos();
        enemyDeath.push_back(enemy);
        m_vgaGfx->draw(*m_animations.fireBallAnimation, SUBPIXEL_TO_PIXEL(enemy.x), SUBPIXEL_TO_PIXEL(enemy.y));
    }

    for (int i = 0; i < m_guffins.size(); ++i)
    {
        Rectangle& guffin = m_guffins[i];
        m_vgaGfx->draw(*m_animations.guffinAnimation, SUBPIXEL_TO_PIXEL(guffin.x), SUBPIXEL_TO_PIXEL(guffin.y));
    }

    for (int i = 0; i < m_jetPacks.size(); ++i)
    {
        Rectangle& jetPack = m_jetPacks[i];
        m_vgaGfx->draw(*m_animations.jetPackAnimation, SUBPIXEL_TO_PIXEL(jetPack.x), SUBPIXEL_TO_PIXEL(jetPack.y));
    }

    for (int i = 0; i < m_sunItems.size(); ++i)
    {
        Rectangle& sunItem = m_sunItems[i];
        m_vgaGfx->draw(*m_animations.jetPackAnimation, SUBPIXEL_TO_PIXEL(sunItem.x), SUBPIXEL_TO_PIXEL(sunItem.y));
    }

    // iterate over boss1 instances and call walk() on them
    for (int i = 0; i < m_boss1.size(); ++i)
    {
        Boss1* boss1Ptr = m_boss1[i];
        boss1Ptr->walk(Rectangle(playerX, playerY, 1, 1));
        tnd::vector<Rectangle> projectiles = boss1Ptr->getProjectiles();

        // draw projectiles
        for (int j = 0; j < projectiles.size(); ++j)
        {
            Rectangle& projectile = projectiles[j];
            enemyDeath.push_back(projectile);
            m_vgaGfx->draw(*m_animations.fireBallAnimation, SUBPIXEL_TO_PIXEL(projectile.x), SUBPIXEL_TO_PIXEL(projectile.y));
        }
    }

    m_vgaGfx->draw(*m_animations.actorAnimation, SUBPIXEL_TO_PIXEL(playerX), SUBPIXEL_TO_PIXEL(playerY));

    m_vgaGfx->drawScreen();
    
    m_physics->setEnemyDeath(enemyDeath);

    ++m_frames;

    KeyBits keys = m_keyMapper.getKeys();

    if (keys & KEY_RIGHT)
    {
        m_physics->setActorSpeedX(m_player, 16);
    }
    
    if (keys & KEY_LEFT)
    {
        m_physics->setActorSpeedX(m_player, -16);
    }

    bool buttonPressed = keys & KEY_JUMP;
    if (buttonPressed && !m_lastButtonPressed)
    {
        m_physics->startActorJump(m_player);
    }
    m_lastButtonPressed = buttonPressed;

    if (buttonPressed && m_jetpackCollected > 0)
    {
        m_physics->activateJetpack(m_player);
    }

    if (keys & KEY_DOWN)
    {
        m_physics->setActorDuck(m_player, true);
    }
    else
    {
        m_physics->setActorDuck(m_player, false);
    }
    
    if (m_sunItemCollected > 0 && (keys & KEY_ACTION1))
    {
        m_physics->activateSunPull(m_player);
    }

    if ((m_frames & 3) == 0)
    {
        m_animations.actorAnimation->nextFrame();
        m_animations.enemyAnimation->nextFrame();
        m_animations.seekerEnemyAnimation->nextFrame();
        m_animations.fireBallAnimation->nextFrame();
        m_animations.jetPackAnimation->nextFrame();
        m_animations.projectileAnimation->nextFrame();
        m_animations.tentacleArmAnimation->nextFrame();
    }

    if ((m_frames & 15) == 0)
    {
        m_animations.guffinAnimation->nextFrame();
        m_animations.tentacleAnimation->nextFrame();
    }

    m_physics->calc();
}

void Game::levelTransition(LevelTransition transition)
{
    switch(transition)
    {
        case RIGHT:
            m_nextLevel = m_levelNumber;
            m_nextLevel.x += 1;
            break;
        case LEFT:
            m_nextLevel = m_levelNumber;
            m_nextLevel.x -= 1;
            break;
        case BOTTOM:
            m_nextLevel = m_levelNumber;
            m_nextLevel.y += 1;
            break;
        case TOP:
            m_nextLevel = m_levelNumber;
            m_nextLevel.y -= 1;
            break;
    }
}
