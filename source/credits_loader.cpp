#include "credits_loader.h"

#include <stdio.h>
#include <string.h>

CreditsLoader::CreditsLoader(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        throw Exception("Could not open file:", filename);
    }

    char line[256];
    while (fgets(line, sizeof(line), file))
    {
        int len = strlen(line);
        if (len > 0)
        {
            if (line[len - 1] == '\n')
            {
                line[len - 1] = 0;
            }
            CreditsEntry entry;
            switch (line[0])
            {
            case '#':
                entry.type = CREDITS_TYPE_ROLE;
                entry.text = (line + 1);
                break;
            case '-':
                entry.type = CREDITS_TYPE_SECTION_END;
                entry.text = "";
                break;
            default:
                entry.type = CREDITS_TYPE_PERSON;
                entry.text = line;
                break;
            }

            // remove trailing '\n' or '\r'
            for (int i = 0; i < entry.text.size(); ++i)
            {
                if (entry.text[i] == '\n' || entry.text[i] == '\r')
                {
                    entry.text[i] = 0;
                    break;
                }
            }

            m_entries.push_back(entry);
        }
    }

    fclose(file);
}

CreditsLoader::~CreditsLoader()
{
}

CreditsEntry &CreditsLoader::getEntry(int index)
{
    return m_entries[index];
}

int CreditsLoader::size() const
{
    return m_entries.size();
}
