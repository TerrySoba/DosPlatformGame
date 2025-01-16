#include "vgagfx.h"
#include "exception.h"
#include "i18n.h"
#include "keyboard.h"
#include "font.h"
#include "font_writer.h"
#include "animation.h"
#include "tga_image.h"
#include "vector.h"
#include "tiny_string.h"

#include <string.h>
#include <stdio.h>


#define TEXT_CENTER_X 120


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
        TgaImage image("credits.tga");
        Font font("lib18.stf");
        Font titleFont("title.stf");
        FontWriter fontWriter(&font);
        FontWriter fontTitleWriter(&titleFont);
        

        vga.setBackground(image);

        fontWriter.setText("Torsten Stremlau");
        fontTitleWriter.setText("- Programming -");
        
        int16_t textYInit = -1 * fontWriter.height();
        int16_t textY = textYInit;
        

        while(!s_keyEsc)
        {
            vga.clear();
            vga.draw(fontWriter, TEXT_CENTER_X - fontWriter.width() / 2, textY + 30);
            vga.draw(fontTitleWriter, TEXT_CENTER_X - fontTitleWriter.width() / 2, textY++);
            vga.drawScreen();

            if (textY > 200)
                textY = textYInit;
        }
    }
    catch(const Exception& e)
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
