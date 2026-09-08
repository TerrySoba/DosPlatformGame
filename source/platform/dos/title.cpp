#include "platform/dos/vgagfx.h"
#include "tga_image.h"
#include "platform/dos/keyboard_dos.h"
#include "font.h"
#include "font_writer.h"
#include "animation.h"
#include "exception.h"
#include "i18n.h"
#include "game_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <exception>
#include <string.h>

#include <vector>

enum ExitCode
{
    EXIT_CODE_NONE = 0,
    EXIT_CODE_START_GAME = 1,
    EXIT_CODE_QUIT = 2
};

// Global variables
bool s_exitRequested = false;
ExitCode s_exitCode = EXIT_CODE_NONE;


enum TitleScreenState
{
    TITLE_SCREEN_STATE_INITIAL,
    TITLE_SCREEN_STATE_MAIN,
    TITLE_SCREEN_STATE_SETTINGS,
    TITLE_SCREEN_STATE_CONFIGURE_KEYBOARD_INITIAL,
    TITLE_SCREEN_STATE_CONFIGURE_KEYBOARD_WAIT_FOR_KEYPRESS,
    TITLE_SCREEN_STATE_CONFIGURE_KEYBOARD_NEXT_KEY,
    TITLE_SCREEN_STATE_CONFIGURE_KEYBOARD_DONE,
};

TitleScreenState s_previousTitleScreenState = TITLE_SCREEN_STATE_INITIAL;
TitleScreenState s_titleScreenState = TITLE_SCREEN_STATE_MAIN;


void deleteSavegame()
{
    remove("GAME.SAV");
}

void requestExit()
{
    s_exitRequested = true;
    s_exitCode = EXIT_CODE_QUIT;
}

void startGame()
{
    s_exitCode = EXIT_CODE_START_GAME;
    s_exitRequested = true;
}

void showSettings()
{
    s_titleScreenState = TITLE_SCREEN_STATE_SETTINGS;
}

void showMainMenu()
{
    s_titleScreenState = TITLE_SCREEN_STATE_MAIN;
}


void showConfigureKeyboard()
{
    s_titleScreenState = TITLE_SCREEN_STATE_CONFIGURE_KEYBOARD_INITIAL;
}

typedef void (*ActionFunction)();


struct MenuItem
{
    uint16_t stringId;
    ActionFunction action;
};


const MenuItem settingsMenuItems[] = {
    { 54, showConfigureKeyboard }, // configure keyboard
    { 53, showMainMenu }, // back
    { 0, NULL } // must end with a NULL entry
};


const MenuItem mainMenuItems[] = {
    { 42, startGame }, // start game
    { 52, showSettings }, // settings
    { 43, deleteSavegame }, // delete savegame
    { 44, requestExit }, // exit
    { 0, NULL } // must end with a NULL entry
};


class MenuSystem
{
public:
    enum MenuState
    {   
        MENU_STATE_MAIN,
        MENU_STATE_ARROW_ANIMATION,
    };

    MenuSystem(VgaGfx& gfx, FontWriter& fontWriter, Drawable& indicator, const MenuItem* menuItems, uint16_t x, uint16_t y) :
        m_gfx(gfx),
        m_fontWriter(fontWriter),
        m_indicator(indicator),
        m_menuItems(menuItems),
        m_activeMenuItemIndex(0),
        m_lastKeyUp(0),
        m_lastKeyDown(0),
        m_lastKeyAction(0),
        m_state(MENU_STATE_MAIN),
        m_indicatorOffsetX(0),
        m_nextAction(NULL),
        m_menuPositionX(x),
        m_menuPositionY(y)
    {
        const MenuItem *menuItem = m_menuItems;
        while (menuItem->stringId != 0)
        {
            m_fontWriter.setText(I18N::getString(menuItem->stringId).c_str());
            m_menuItemPositionsY.push_back(y);
            y += m_fontWriter.height();
            ++menuItem;
        }
    }

    void drawBackground()
    {
        const MenuItem *menuItem = m_menuItems;
        size_t menuItemIndex = 0;
        while (menuItem->stringId != 0)
        {
            m_fontWriter.setText(I18N::getString(menuItem->stringId).c_str());
            m_gfx.drawBackground(m_fontWriter, m_menuPositionX, m_menuItemPositionsY[menuItemIndex]);
            ++menuItemIndex;
            ++menuItem;
        }
    }

    void drawActiveItemIndicator()
    {
        if (m_state == MENU_STATE_ARROW_ANIMATION)
        {
            if (m_indicatorOffsetX == 0)
            {
                m_indicatorOffsetX = 16;
            }
            else
            {
                m_indicatorOffsetX *= 1.15;
            }

            if ((m_indicatorOffsetX >> 2) > 120)
            {
                m_indicatorOffsetX = 0;
                m_state = MENU_STATE_MAIN;
                if (m_nextAction != NULL)
                {
                    m_nextAction();
                }
            }
        }


        if (m_activeMenuItemIndex < m_menuItemPositionsY.size())
        {
            uint16_t y = m_menuItemPositionsY[m_activeMenuItemIndex];
            m_gfx.draw(m_indicator, m_menuPositionX - 15 + (m_indicatorOffsetX >> 2), y - 1);
        }
    }

    void handleKeyboardInputs()
    {
        if (m_state == MENU_STATE_ARROW_ANIMATION)
        {
            return;
        }

        uint8_t keyUp = s_keyUp;
        uint8_t keyDown = s_keyDown;
        uint8_t keyAction = s_keySpace | s_keyCtrl | s_keyAlt;

        if (keyUp && !m_lastKeyUp)
        {
            if (m_activeMenuItemIndex > 0)
            {
                --m_activeMenuItemIndex;
            }
        }
        else if (keyDown && !m_lastKeyDown)
        {
            if (m_activeMenuItemIndex + 1 < m_menuItemPositionsY.size())
            {
                ++m_activeMenuItemIndex;
            }
        }
        else if (keyAction && !m_lastKeyAction)
        {
            const MenuItem* menuItem = &m_menuItems[m_activeMenuItemIndex];
            if (menuItem->action != NULL)
            {
                m_nextAction = menuItem->action;
                m_state = MENU_STATE_ARROW_ANIMATION;
            }
        }

        m_lastKeyUp = keyUp;
        m_lastKeyDown = keyDown;
        m_lastKeyAction = keyAction;
    }

private:
    VgaGfx& m_gfx;
    FontWriter& m_fontWriter;
    Drawable& m_indicator;
    const MenuItem* m_menuItems;
    uint16_t m_activeMenuItemIndex;
    std::vector<uint16_t> m_menuItemPositionsY;
    uint16_t m_menuPositionX;
    uint16_t m_menuPositionY;

    uint8_t m_lastKeyUp;
    uint8_t m_lastKeyDown;
    uint8_t m_lastKeyAction;

    MenuState m_state;
    uint16_t m_indicatorOffsetX;

    ActionFunction m_nextAction;
};

struct KeyMapping
{
    uint16_t textId;
    uint8_t* keyStorage;
};


KeyboardConfig s_keyboardConfig;

const KeyMapping keyMappings[] = {
    {55, &s_keyboardConfig.keyUp},
    {56, &s_keyboardConfig.keyDown},
    {57, &s_keyboardConfig.keyLeft},
    {58, &s_keyboardConfig.keyRight},
    {59, &s_keyboardConfig.keyJump},
    {60, &s_keyboardConfig.keyAction},
    {0, 0}
};

class ConfigureKeyboard
{
public:
    ConfigureKeyboard(VgaGfx& gfx, FontWriter& fontWriterText, FontWriter& fontWriterAction, uint16_t x, uint16_t y) :
        m_gfx(gfx),
        m_fontWriterText(fontWriterText),
        m_fontWriterAction(fontWriterAction),
        m_x(x),
        m_y(y)
    {
    }

    void drawBackground()
    {
        m_fontWriterText.setText(I18N::getString(61).c_str()); // "Press key for action:"
        m_gfx.drawBackground(m_fontWriterText, m_x, m_y);
        m_fontWriterAction.setText(I18N::getString(keyMappings[m_currentKeyMappingIndex].textId).c_str());
        m_gfx.drawBackground(m_fontWriterAction, m_x + 30, m_y + 15);
    }

    void handleKeyboardInputs()
    {
        if (s_keyIsPressed)
        {
            *keyMappings[m_currentKeyMappingIndex].keyStorage = s_scancode;
            m_currentKeyMappingIndex++;
            if (keyMappings[m_currentKeyMappingIndex].keyStorage != 0)
            {
                s_titleScreenState = TITLE_SCREEN_STATE_CONFIGURE_KEYBOARD_NEXT_KEY;
            }
            else
            {
                m_currentKeyMappingIndex = 0;
                s_titleScreenState = TITLE_SCREEN_STATE_CONFIGURE_KEYBOARD_DONE;
                writeKeyboardConfig(DEFAULT_CONFIG_NAME, s_keyboardConfig);
            }
        }
    }
    
    void reset()
    {
        m_currentKeyMappingIndex = 0;
    }

private:
    VgaGfx& m_gfx;
    FontWriter& m_fontWriterText;
    FontWriter& m_fontWriterAction;
    uint16_t m_x;
    uint16_t m_y;
    size_t m_currentKeyMappingIndex;
};



void drawVersionNumber(VgaGfx& gfx)
{
    Font font8("geo10.stf");
    FontWriter fontWriter2(&font8);
    fontWriter2.setText(BUILD_VERSION);
    gfx.drawBackground(fontWriter2, 320 - fontWriter2.width() - 3, 1);
}

#define NEW_KEY_WAIT_FRAMES 20

int main(int argc, char* argv[])
{
    bool useGerman = false;

    for (int i = 1; i < argc; ++i)
    {
        if (strcmp("--german", argv[i]) == 0)	
        {
            useGerman = true;
        }
    }

    try
    {
        if (useGerman) I18N::loadTranslations("strings.de");
        else I18N::loadTranslations("strings.en");

        Keyboard keys;
        VgaGfx vga;
        Animation arrow("arrow2.ani", "arrow2.tga", true);
        TgaImage image("pyramid.tga");
        Font mainFont("a13.stf");
        FontWriter mainFontWriter(&mainFont);
        Font settingsFont("com10.stf");
        FontWriter settingsFontWriter(&settingsFont);

        MenuSystem menu(vga, mainFontWriter, arrow, mainMenuItems, 190, 87);
        MenuSystem settingsMenu(vga, settingsFontWriter , arrow, settingsMenuItems, 190, 87);
        ConfigureKeyboard configureKeyboard(vga, settingsFontWriter, mainFontWriter, 190, 87);
        
        uint8_t counter = 0;

        uint16_t newKeyWaitFrames = NEW_KEY_WAIT_FRAMES;

        while (!s_keyEsc && !s_exitRequested)
        { 
            vga.clear();

            if (s_titleScreenState != s_previousTitleScreenState)
            {
                // clear screen when state changes
                vga.setBackground(image);
                drawVersionNumber(vga);

                switch (s_titleScreenState)
                {
                case TITLE_SCREEN_STATE_MAIN:
                    menu.drawBackground();
                    break;
                case TITLE_SCREEN_STATE_SETTINGS:
                    settingsMenu.drawBackground();
                    break;
                case TITLE_SCREEN_STATE_CONFIGURE_KEYBOARD_INITIAL:
                    configureKeyboard.reset();
                    configureKeyboard.drawBackground();
                    s_titleScreenState = TITLE_SCREEN_STATE_CONFIGURE_KEYBOARD_WAIT_FOR_KEYPRESS;
                    break;
                case TITLE_SCREEN_STATE_CONFIGURE_KEYBOARD_WAIT_FOR_KEYPRESS:
                    configureKeyboard.drawBackground();
                    break;
                }

                s_previousTitleScreenState = s_titleScreenState;
            }

            switch (s_titleScreenState)
            {
            case TITLE_SCREEN_STATE_MAIN:
                menu.drawActiveItemIndicator();
                menu.handleKeyboardInputs();
                break;
            case TITLE_SCREEN_STATE_SETTINGS:
                settingsMenu.drawActiveItemIndicator();
                settingsMenu.handleKeyboardInputs();
                break;
            case TITLE_SCREEN_STATE_CONFIGURE_KEYBOARD_WAIT_FOR_KEYPRESS:
                configureKeyboard.handleKeyboardInputs();
                break;
            case TITLE_SCREEN_STATE_CONFIGURE_KEYBOARD_NEXT_KEY:
                if (newKeyWaitFrames == 0)
                {
                    newKeyWaitFrames = NEW_KEY_WAIT_FRAMES;
                    s_titleScreenState = TITLE_SCREEN_STATE_CONFIGURE_KEYBOARD_WAIT_FOR_KEYPRESS;
                }
                else
                {
                    --newKeyWaitFrames;
                }
                break;
            case TITLE_SCREEN_STATE_CONFIGURE_KEYBOARD_DONE:
                if (newKeyWaitFrames == 0)
                {
                    newKeyWaitFrames = NEW_KEY_WAIT_FRAMES;
                    s_titleScreenState = TITLE_SCREEN_STATE_SETTINGS;
                }
                else
                {
                    --newKeyWaitFrames;
                }
                break;
            }
            vga.drawScreen();
            
            
            ++counter;
            if (counter > 1)
            {
                arrow.nextFrame();
                counter = 0;
            }
        }

        return s_exitCode;
    }
    catch(const Exception& e)
    {
        fprintf(stderr, "Exception: %s\n", e.what());
        return 1;
    }
    catch(const std::exception& e)
    {
        fprintf(stderr, "Exception: %s\n", e.what());
        return 1;
    }
    catch(...)
    {
        fprintf(stderr, "Unknown exception.");
        return 1;
    }
    
    return 0;
}