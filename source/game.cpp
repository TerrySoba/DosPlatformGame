#include "game.h"
#include "keyboard.h"
#include "level.h"
#include "image_base.h"
#include "animation.h"
#include "vgagfx.h"
#include "physics.h"
#include "version.h"
#include "vector.h"
#include "exception.h"
#include "compiled_sprite.h"
#include "joystick.h"
#include "game_save.h"
#include "text.h"
#include "i18n.h"
#include "tile_definitions.h"
#include "fire_ball.h"
#include "seeker_enemy.h"

#include <stdio.h>

Game::Game(shared_ptr<VgaGfx> vgaGfx, shared_ptr<SoundController> sound, 
           shared_ptr<ImageBase> tiles,
           GameAnimations animations,
           const char* levelBasename, LevelNumber startLevel) :
    m_vgaGfx(vgaGfx), m_tiles(tiles), m_animations(animations), m_frames(0), m_levelBasename(levelBasename),
    m_animationController(animations.actorAnimation, sound), m_lastButtonPressed(false), m_sound(sound),
    m_jetpackCollected(0), m_button1(0), m_levelMustReload(false)
{
    m_nextLevel.x = -1;
    m_nextLevel.y = -1;

    m_appleString = I18N::getString(2);

    // try to load savegame
    GameState state;
    if (loadGameState(state, "game.sav"))
    {
        m_collectedGuffins = state.colectedGuffins;
        m_jetpackCollected = state.jetpackCollected;
        m_button1 = state.button1;
        loadLevel(state.level, ActorPosition::LevelTransition);   
        m_physics->setSpawnPoint(state.spawnPoint);
        drawAppleCount();
    }
    else
    {
        loadLevel(startLevel, ActorPosition::UseSpawnPoint);
    }
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

    sprintf(buf, m_levelBasename.c_str(), m_levelNumber.x, m_levelNumber.y);

    TinyString levelMap = TinyString(buf + TinyString(".map"));

    if (m_button1 == 1)
    {
        TinyString buttonLevelName = TinyString(buf + TinyString("b1.map"));
        if (fileExists(buttonLevelName.c_str()))
        {
            levelMap = buttonLevelName;
        }
    }

    Level level(levelMap.c_str(), m_tiles, 16, 16, -8, -8);
    m_animations.actorAnimation->useTag("LoopR");

    int16_t actorPosX, actorPosY;
    if (actorPosition == ActorPosition::KeepActorPos)
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
    m_enemies.clear();
    tnd::vector<Rectangle> enemyRectangles = level.getEnemies();
    for (int i = 0; i < enemyRectangles.size(); ++i)
    {
        m_enemies.push_back(shared_ptr<Enemy>(new Enemy(enemyRectangles[i], m_animations.enemyAnimation)));
    }

    // now load seeker enemies
    m_seekerEnemies.clear();
    tnd::vector<Rectangle> seekerEnemyRectangles = level.getSeekerEnemies();
    for (int i = 0; i < seekerEnemyRectangles.size(); ++i)
    {
        m_seekerEnemies.push_back(shared_ptr<SeekerEnemy>(new SeekerEnemy(seekerEnemyRectangles[i], m_animations.seekerEnemyAnimation)));
    }


    // now load fireballs if any exist
    m_fireBalls.clear();
    tnd::vector<Rectangle> fireBalls = level.getFireBalls();
    for (int i = 0; i < fireBalls.size(); ++i)
    {
        m_fireBalls.push_back(shared_ptr<FireBall>(new FireBall(fireBalls[i], m_animations.fireBallAnimation)));
    }


    m_guffins.clear();
    m_guffins = level.getMacGuffins();


    m_jetPacks.clear();
    if (m_jetpackCollected == 0) // only load jetpack elements if it has not been collected yet
    {
        m_jetPacks = level.getJetPacks();
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


    m_physics.reset(); // reset first, so we do not have two instances of physics at once
    m_physics = shared_ptr<Physics>(new Physics(this, m_sound));
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
            if (mapData[i] == GFX_TILE_SWITCH_1) {
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

    
    m_physics->setSpawnPoint(Point(actorPosX, actorPosY));

    m_vgaGfx->clear();

    
    m_vgaGfx->drawBackground(level, -8, -8);

    tnd::vector<MessageBox> messageBoxes = level.getMessageBoxes();
    for (int i = 0; i < messageBoxes.size(); ++i) {
        MessageBox& messageBox = messageBoxes[i];
        TinyString message = I18N::getString(messageBox.textId);
        Text t(message.c_str(), messageBox.w / 5, true);
        m_vgaGfx->drawBackground(t, messageBox.x, messageBox.y);
    }

    TinyString levelString = I18N::getString((m_levelNumber.y << 6) + m_levelNumber.x);
    Text t(levelString.c_str(), 0, false);
    m_vgaGfx->drawBackground(t, 50, 193);
    drawAppleCount();

    // auto save current state
    GameState state;
    state.level = m_levelNumber;
    state.spawnPoint = Point(actorPosX, actorPosY);
    state.colectedGuffins = m_collectedGuffins;
    state.jetpackCollected = m_jetpackCollected;
    state.button1 = m_button1;
    saveGameState(state, "game.sav");
}

void Game::drawAppleCount()
{
    char buf[16];
    snprintf(buf, 16, m_appleString.c_str(), m_collectedGuffins.size());
    Text t(buf);
    m_vgaGfx->drawBackground(t, 260, 1);
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

void Game::touchButton(uint16_t id, ButtonType type)
{
    if (id == 1 && type == BUTTON_ON && m_button1 == 0)
    {
        m_button1 = 1;
        m_sound->playJetpackSound();
        m_levelMustReload = true; // mark level to be reloaded in text frame
    }
}

void Game::drawFrame()
{
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
        m_enemies[i]->walk();
        Rectangle enemy = m_enemies[i]->getPos();
        enemyDeath.push_back(enemy);
        m_vgaGfx->draw(*m_animations.enemyAnimation, SUBPIXEL_TO_PIXEL(enemy.x), SUBPIXEL_TO_PIXEL(enemy.y));
    }

    for (int i = 0; i < m_seekerEnemies.size(); ++i)
    {
        m_seekerEnemies[i]->walk(Rectangle(playerX, playerY, 1, 1));
        Rectangle enemy = m_seekerEnemies[i]->getPos();
        enemyDeath.push_back(enemy);

        // LOG_ENTRY("x:%d y:%d", enemy.x, enemy.y);

        m_vgaGfx->draw(*m_animations.seekerEnemyAnimation, SUBPIXEL_TO_PIXEL(enemy.x), SUBPIXEL_TO_PIXEL(enemy.y));
    }

    for (int i = 0; i < m_fireBalls.size(); ++i)
    {
        m_fireBalls[i]->walk();
        Rectangle enemy = m_fireBalls[i]->getPos();
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


    m_vgaGfx->draw(*m_animations.actorAnimation, SUBPIXEL_TO_PIXEL(playerX), SUBPIXEL_TO_PIXEL(playerY));

    m_vgaGfx->drawScreen();
    
    m_physics->setEnemyDeath(enemyDeath);

    ++m_frames;

    uint8_t joystick = readJoystick();


    if (s_keyRight || joystick & JOY_RIGHT)
    {
        m_physics->setActorSpeedX(m_player, 16);
    }
    
    if (s_keyLeft || joystick & JOY_LEFT)
    {
        m_physics->setActorSpeedX(m_player, -16);
    }

    bool buttonPressed = s_keyAlt || joystick & JOY_BUTTON_1;
    if (buttonPressed && !m_lastButtonPressed)
    {
        m_physics->startActorJump(m_player);
    }
    m_lastButtonPressed = buttonPressed;

    if (buttonPressed && m_jetpackCollected > 0)
    {
        m_physics->activateJetpack(m_player);
    }

    if (s_keyDown || joystick & JOY_DOWN)
    {
        m_physics->setActorDuck(m_player, true);
    }
    else
    {
        m_physics->setActorDuck(m_player, false);
    }
    

    if (m_frames % 4 == 0)
    {
        m_animations.actorAnimation->nextFrame();
        m_animations.enemyAnimation->nextFrame();
        m_animations.seekerEnemyAnimation->nextFrame();
        m_animations.fireBallAnimation->nextFrame();
        m_animations.jetPackAnimation->nextFrame();
    }

    if (m_frames % 16 == 0)
    {
        m_animations.guffinAnimation->nextFrame();
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
