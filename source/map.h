#ifndef _MAP_H_INCLUDED
#define _MAP_H_INCLUDED


#include <stdio.h>

namespace tnd
{



template <class FirstT, class SecondT>
class pair
{
public:
    FirstT first;
    SecondT second;
};


template <class KeyT, class ValueT>
class map
{
public:

    map()
    {
        m_entries = new pair<KeyT, ValueT>[1];
        m_end = m_entries;
    }


    ValueT& operator[](const KeyT& key)
    {
        {
            iterator it = find(key);
            if (it != end())
            {
                return it->second;
            }
        }

        // No matching entry was found. Create new empty entry.
        

        // Find correct place to insert new key
        iterator item;
        for (item = begin(); item != end(); ++item)
        {
            if (item->first > key)
            {
                break;
            }
        }

        int newPos = item - begin();

        // allocate new data
        int s = size() + 1;
        pair<KeyT, ValueT>* data = new pair<KeyT, ValueT>[s + 1];
        iterator dest = data;
        for (iterator it = begin(); it != item; ++it)
        {
            *dest++ = *it;
        }

        dest->first = key;
        ++dest;

        if (item != end())
        {
            for (iterator it = item; it != end(); ++it)
            {
                *dest++ = *it;
            }
        }

        delete[] m_entries;

        m_entries = data;
        m_end = m_entries + s;

        return m_entries[newPos].second;
    }

    int size() const
    {
        return m_end - m_entries;
    }

    typedef pair<KeyT, ValueT>* iterator;

    iterator begin()
    {
        return m_entries;
    }

    iterator end()
    {
        return m_end;
    }

    iterator find(const KeyT& key)
    {
        // Currently we are using a linear search here. That is not very efficient and should be improved.
        iterator it = begin();

        while (it != end() && it->first != key)
        {
            ++it;
        }

        return it;
    }

private:
    pair<KeyT, ValueT>* m_entries;
    pair<KeyT, ValueT>* m_end;
};


}

#endif