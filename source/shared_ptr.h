#ifndef SHARED_PTR_INCLUDED
#define SHARED_PTR_INCLUDED

#ifdef __GNUC__
#include <memory>
#endif

namespace tnd // TaNDy game
{


#ifdef __GNUC__

template <class T>
using shared_ptr = std::shared_ptr<T>;

#else

template <class T>
struct shared_ptr_storage
{
    int user_count;
    T* ptr;
};


template <class ValueT>
class shared_ptr
{
public:
    shared_ptr() : m_storage((shared_ptr_storage<ValueT>*)0)
    {
    }

    shared_ptr(ValueT* ptr)
    {
        if (ptr)
        {
            m_storage = new shared_ptr_storage<ValueT>;
            m_storage->user_count = 1;
            m_storage->ptr = ptr;
        } else {
            m_storage = (shared_ptr_storage<ValueT>*)0;
        }
    }

    shared_ptr(const shared_ptr<ValueT>& other)
    {
        m_storage = other.m_storage;
        m_storage->user_count++;
    }

    shared_ptr<ValueT>& operator=(const shared_ptr<ValueT>& other)
    {
        if (this == &other)
        {
            return *this;
        }
        
        delete_storage();
        m_storage = other.m_storage;
        m_storage->user_count++;
        return *this;
    }

    ~shared_ptr()
    {
        delete_storage();
    }

    void reset(ValueT* ptr = 0)
    {
        delete_storage();
        m_storage = (shared_ptr_storage<ValueT>*)0;
        if (ptr)
        {
            *this = shared_ptr<ValueT>(ptr);
        }
    }

    ValueT* get()
    {
        if (!m_storage)
        {
            return (ValueT*)0;
        }
        return m_storage->ptr;
    }

    int use_count()
    {
        if (!m_storage)
        {
            return 0;
        }
        return m_storage->user_count;
    }

    ValueT* operator->()
    {
        return m_storage->ptr;
    }

    const ValueT* operator->() const
    {
        return m_storage->ptr;
    }

    ValueT& operator*()
    {
        return *m_storage->ptr;
    }

    const ValueT& operator*() const
    {
        return *m_storage->ptr;
    }

private:
    void delete_storage()
    {
        if (m_storage)
        {
            if (m_storage->user_count == 1)
            {
                delete m_storage->ptr;
                delete m_storage;
            }
            else
            {
                m_storage->user_count--;
            }
        }
    }

private:
    shared_ptr_storage<ValueT>* m_storage;
};

#endif

} // namespace tnd

#endif
