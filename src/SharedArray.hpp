// SharedArray.hpp
// by Matthew Spellings <mspells@umich.edu>

#ifndef __SHAREDARRAY_HPP_
#define __SHAREDARRAY_HPP_

#include <algorithm>

template<typename T> class SharedArray;

template<typename T>
class SharedArrayShim
{
    friend class SharedArray<T>;
public:
    SharedArrayShim(T *target, size_t length):
        m_target(target),
        m_length(length),
        m_count(1)
    {}

    void increment()
    {
        ++m_count;
    }

    void decrement()
    {
        --m_count;
        if(!m_count)
        {
            m_length = 0;
            delete[] m_target;
            m_target = NULL;
        }
    }

private:
    T *m_target;
    size_t m_length;
    size_t m_count;
};

template<typename T>
class SharedArray
{
public:
    typedef T* iterator;

    SharedArray():
        m_shim(NULL)
    {}

    SharedArray(T *target, size_t length):
        m_shim(new SharedArrayShim<T>(target, length))
    {}

    SharedArray(const SharedArray<T> &rhs):
        m_shim(rhs.m_shim)
    {
        if(m_shim)
            m_shim->increment();
    }

    ~SharedArray()
    {
        release();
    }

    bool isNull()
    {
        return m_shim == NULL || m_shim->target == NULL;
    }

    iterator begin()
    {
        return get();
    }

    iterator end()
    {
        return get() + size();
    }

    T *get()
    {
        if(m_shim)
            return m_shim->m_target;
        else
            return NULL;
    }

    size_t size() const
    {
        if(m_shim)
            return m_shim->m_length;
        else
            return 0;
    }

    void release()
    {
        if(m_shim)
        {
            m_shim->decrement();
            if(m_shim->m_target == NULL)
                delete m_shim;
        }
        m_shim = NULL;
    }

    // Stop managing this array and give it to C.
    T *disown()
    {
        T *result(NULL);
        if(m_shim)
        {
            result = m_shim->m_target;
            delete m_shim;
        }
        return result;
    }

    void swap(SharedArray<T> &target)
    {
        std::swap(m_shim, target.m_shim);
    }

    T &operator[](size_t idx)
    {
        return m_shim->m_target[idx];
    }

    const T &operator[](size_t idx) const
    {
        return m_shim->m_target[idx];
    }

private:
    SharedArrayShim<T> *m_shim;
};

#endif
