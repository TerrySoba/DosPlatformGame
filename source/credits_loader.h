#ifndef _CREDITS_LOADER_H
#define _CREDITS_LOADER_H

#include "tiny_string.h"
#include "vector.h"
#include "exception.h"
#include "tiny_string.h"

#include <stdio.h>
#include <string.h>

enum CreditsType
{
    CREDITS_TYPE_ROLE = 1,
    CREDITS_TYPE_PERSON = 2,
    CREDITS_TYPE_SECTION_END = 3
};

struct CreditsEntry
{
    TinyString text;
    CreditsType type;
};


class CreditsLoader
{
public:
    CreditsLoader(const char* filename)
    {
        FILE* file = fopen(filename, "r");
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
                switch(line[0])
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

                m_entries.push_back(entry);
            }
        }
        
        fclose(file);
    }

    ~CreditsLoader()
    {
        
    }

    CreditsEntry& getEntry(int index)
    {
        return m_entries[index];
    }

    int size() const
    {
        return m_entries.size();
    }


private:
    tnd::vector<CreditsEntry> m_entries;

};


#endif
