#ifndef RAII_PTR_H_INCLUDED
#define RAII_PTR_H_INCLUDED


template <class ValueT>
class raii_ptr
{
public:
    raii_ptr(ValueT* ptr) : ptr(ptr) {}
    ~raii_ptr() { delete ptr; }

    ValueT* get() { return ptr; }

    ValueT* operator->()
    {
        return ptr;
    }

    const ValueT* operator->() const
    {
        return ptr;
    }

private:
    ValueT* ptr;
};

#endif