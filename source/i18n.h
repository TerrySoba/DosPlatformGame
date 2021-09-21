#ifndef _I18N_INCLUDED
#define _I18N_INCLUDED

#include "tiny_string.h"
#include <stdint.h>
#include <stdio.h>

struct StringEntry;

class I18N
{
public:
    static void loadTranslations(const char* path);
    static TinyString getString(uint16_t id);

private:
    I18N(const char* path);
    ~I18N();
    TinyString _getString(uint16_t id);

private: // static members
    static I18N* s_instance;
    
private:
    uint16_t m_entryCount;
    StringEntry* m_entries;
    FILE* m_file;
};


#endif