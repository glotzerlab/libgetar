# distutils: language = c++
# distutils: sources = src/SharedArray.hpp

from libcpp cimport bool

cdef extern from "../src/SharedArray.hpp" namespace "gtar":
    cdef cppclass SharedArray[T]:
        # ctypedef (T*) iterator

        SharedArray()
        SharedArray(T*, size_t)
        SharedArray(const SharedArray[T]&)
        # ~SharedArray()

        bool isNull()
        T* begin()
        T* end()

        T *get()
        size_t size() const
        void release()
        T *disown()

        void swap(SharedArray[T]&)
        T &operator[]
        const T &operator[] const
