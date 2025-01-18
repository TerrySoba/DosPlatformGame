#ifndef _CREDITS_LOADER_H
#define _CREDITS_LOADER_H

#include "tiny_string.h"
#include "vector.h"
#include "exception.h"
#include "tiny_string.h"

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
    CreditsLoader(const char* filename);
    ~CreditsLoader();

    CreditsEntry& getEntry(int index);

    int size() const;

private:
    tnd::vector<CreditsEntry> m_entries;
};


#endif
