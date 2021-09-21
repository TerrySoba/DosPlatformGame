#include "i18n.h"


#include "exception.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#pragma pack(0);

struct StringEntry
{
    uint16_t id;
    uint32_t offset;
    uint16_t length;
};

I18N* I18N::s_instance = NULL;


void I18N::loadTranslations(const char* path)
{
    if (s_instance) delete s_instance;
    s_instance = new I18N(path);
}

TinyString I18N::getString(uint16_t id)
{
    if (s_instance) return s_instance->_getString(id);
    else return "Uninitialized";
}

I18N::I18N(const char* path)
{
    m_file = fopen(path, "rb");

    if (!m_file)
    {
        throw Exception("Could not open file: ", path);
    }

    char buffer[16];

    fread(buffer, 4, 1, m_file);
    if (memcmp(buffer, "TEXT", 4) != 0)
    {
        throw Exception("I18N file has incorrect header: ", path);
    }

    fread(&m_entryCount, 2, 1, m_file);
    m_entries = new StringEntry[m_entryCount];
    fread(m_entries, sizeof(StringEntry), m_entryCount, m_file);
}

I18N::~I18N()
{
    fclose(m_file);
    delete[] m_entries;
}




TinyString I18N::_getString(uint16_t id)
{
    for (int i = 0; i < m_entryCount; ++i)
    {
        if (m_entries[i].id == id)
        {
            // printf("id: %d\n", entries[i].id);

            fseek(m_file, m_entries[i].offset, SEEK_SET);
            TinyString str(m_entries[i].length);

            fread(str.data(), m_entries[i].length, 1, m_file);
            // printf("str: %s\n", str.c_str());
            return str;
        }
    }

    // If string is not available in translation file, then display three question marks
    // and the id of the missing string.
    char buf[20];
    snprintf(buf, 20, "??? <id:%d>", id);

    return buf;
}
