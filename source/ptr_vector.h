#ifndef PTR_VECTOR_H_INCLUDED
#define PTR_VECTOR_H_INCLUDED

#include "vector.h"

namespace tnd // TaNDy game
{

/**
 * A vector that owns its elements and deletes them when it is destroyed.
 */
template <typename T>
class ptr_vector
{
public:
    ~ptr_vector() {
        for (int i = 0; i < m_vector.size(); ++i)
        {
            delete m_vector[i];
        }
    }

    void push_back(T* ptr)
    {
        m_vector.push_back(ptr);
    }

    T* operator[](int index) const
    {
        return m_vector[index];
    }

    int size() const
    {
        return m_vector.size();
    }

    void clear()
    {
        for (int i = 0; i < m_vector.size(); ++i)
        {
            delete m_vector[i];
        }
        m_vector.clear();
    }

private:
    tnd::vector<T*> m_vector;
};

} // namespace tnd

#endif