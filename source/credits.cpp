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
#include "credits_loader.h"
#include "shared_ptr.h"

#include <string.h>
#include <stdio.h>


#define TEXT_CENTER_X 120


struct TextEntry
{
    tnd::shared_ptr<FontWriter> writer;
    uint16_t x, y;
};

class TextBlock
{
public:
    void draw(VgaGfx& vga, int16_t x, int16_t y)
    {
        for (int i = 0; i < m_entries.size(); ++i)
        {
            vga.draw(*m_entries[i].writer, m_entries[i].x + x, m_entries[i].y + y);
        }
    }

    void addEntry(tnd::shared_ptr<FontWriter> writer, uint16_t x, uint16_t y)
    {
        TextEntry entry;
        entry.writer = writer;
        entry.x = x;
        entry.y = y;
        m_entries.push_back(entry);
    }

private:
    tnd::vector<TextEntry> m_entries;

};


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
        vga.setBackground(image);

        Font personFont("lib18.stf");
        Font titleFont("title.stf");

        CreditsLoader creditsLoader("credits.txt");

        const uint16_t MAX_PERSONS = 4;

        tnd::shared_ptr<FontWriter> personWriters[MAX_PERSONS];
        for (int i = 0; i < MAX_PERSONS; ++i)
        {
            personWriters[i] = tnd::shared_ptr<FontWriter>(new FontWriter(&personFont));
        }
        tnd::shared_ptr<FontWriter> titleWriter = tnd::shared_ptr<FontWriter>(new FontWriter(&titleFont));
        
        bool titlesDone = false;
        
        uint16_t creditsLine = 0;

        while (!titlesDone)
        {
            uint16_t personWriterCount = 0;
            uint16_t titleWriterCount = 0;
            

            while (creditsLine < creditsLoader.size()) 
            {
                CreditsEntry& entry = creditsLoader.getEntry(creditsLine++);
                if (entry.type ==  CREDITS_TYPE_ROLE)
                {
                    if (titleWriterCount < 1)
                    {
                        titleWriter->setText(entry.text.c_str());
                        titleWriterCount++;
                    }
                }
                else if (entry.type == CREDITS_TYPE_PERSON)
                {
                    if (personWriterCount < MAX_PERSONS)
                    {
                        personWriters[personWriterCount]->setText(entry.text.c_str());
                        personWriterCount++;
                    }
                }
                else if (entry.type == CREDITS_TYPE_SECTION_END)
                {
                        break;
                }
            }

            if (creditsLine == creditsLoader.size())
            {
                titlesDone = true;
            }

            // calculate height of text
            int16_t textHeight = 0;
            for (int i = 0; i < personWriterCount; ++i)
            {
                textHeight += personWriters[i]->height();
            }
            if (titleWriterCount > 0)
            {
                textHeight += titleWriter->height();
            }

            TextBlock textBlock;
            int16_t textY = 0;
    
            if (titleWriterCount > 0)
            {
                textBlock.addEntry(titleWriter, TEXT_CENTER_X - titleWriter->width() / 2, textY);
                textY += titleWriter->height();
            }

            for (int i = 0; i < personWriterCount; ++i)
            {
                textBlock.addEntry(personWriters[i], TEXT_CENTER_X - personWriters[i]->width() / 2, textY);
                textY += personWriters[i]->height();
            }

            int16_t y = 200 << 4;

            while ((y  >> 4) > -textHeight)
            {
                vga.clear();
                textBlock.draw(vga, 0, y >> 4);
                vga.drawScreen();
                y -= 6;
            }
            
        }




        // Font font("lib18.stf");
        // Font titleFont("title.stf");
        // FontWriter fontWriter(&font);
        // FontWriter fontTitleWriter(&titleFont);
        

        

        // fontWriter.setText("Torsten Stremlau");
        // fontTitleWriter.setText("- Programming -");
        
        // int16_t textYInit = -1 * fontWriter.height();
        // int16_t textY = textYInit;
        

        // while(!s_keyEsc)
        // {
        //     vga.clear();
        //     vga.draw(fontWriter, TEXT_CENTER_X - fontWriter.width() / 2, textY + 30);
        //     vga.draw(fontTitleWriter, TEXT_CENTER_X - fontTitleWriter.width() / 2, textY++);
        //     vga.drawScreen();

        //     if (textY > 200)
        //         textY = textYInit;
        // }
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
