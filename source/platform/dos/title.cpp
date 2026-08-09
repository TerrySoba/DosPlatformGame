#include "platform/dos/vgagfx.h"
#include "tga_image.h"
#include "platform/dos/keyboard_dos.h"
#include "font.h"
#include "font_writer.h"
#include "animation.h"
#include "exception.h"
#include "i18n.h"

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

typedef void (*ActionFunction)();

struct MenuItem
{
    uint16_t stringId;
    ActionFunction action;
};


const MenuItem menuItems[] = {
    { 42, startGame }, // start game
    // { 52, NULL }, // settings
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

    MenuSystem(VgaGfx& gfx, FontWriter& fontWriter, Drawable& indicator, const MenuItem* menuItems) :
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
        m_nextAction(NULL) {}

    void drawBackground(uint16_t x, uint16_t y)
    {
        m_menuItemPositionsX = x;
        const MenuItem* menuItem = m_menuItems;
        while (menuItem->stringId != 0)
        {
            m_fontWriter.setText(I18N::getString(menuItem->stringId).c_str());
            m_gfx.drawBackground(m_fontWriter, x, y);
            m_menuItemPositionsY.push_back(y);
            y += m_fontWriter.height();
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
            m_gfx.draw(m_indicator, m_menuItemPositionsX - 15 + (m_indicatorOffsetX >> 2), y - 1);
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
    uint16_t m_menuItemPositionsX;

    uint8_t m_lastKeyUp;
    uint8_t m_lastKeyDown;
    uint8_t m_lastKeyAction;

    MenuState m_state;
    uint16_t m_indicatorOffsetX;

    ActionFunction m_nextAction;
};

void drawVersionNumber(VgaGfx& gfx)
{
    Font font8("geo10.stf");
    FontWriter fontWriter2(&font8);
    fontWriter2.setText(BUILD_VERSION);
    gfx.drawBackground(fontWriter2, 320 - fontWriter2.width() - 3, 1);
}



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
        Font font("a13.stf");
        FontWriter fontWriter(&font);
        MenuSystem menu(vga, fontWriter, arrow, menuItems);
        
        vga.setBackground(image);

        menu.drawBackground(190, 87);

        drawVersionNumber(vga);

        uint8_t counter = 0;
        while (!s_keyEsc && !s_exitRequested)
        { 
            vga.clear();
            menu.drawActiveItemIndicator();
            vga.drawScreen();
            
            menu.handleKeyboardInputs();

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