# distutils: language = c++

from libcpp.string cimport string
from libcpp.vector cimport vector
from cython.operator cimport dereference as deref

from libcpp cimport bool

cdef extern from "../src/SharedArray.hpp" namespace "gtar":
    cdef cppclass SharedArray[T]:
        # ctypedef (T*) iterator

        SharedArray()
        SharedArray(T*, size_t)
        SharedArray(const SharedArray[T]&)
        # ~SharedArray()

        void copy(const SharedArray[T]&)
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

cdef extern from "../src/Archive.hpp" namespace "gtar":
    cdef enum OpenMode:
        Read
        Write
        Append

    cdef enum CompressMode:
        NoCompress
        FastCompress
        MediumCompress
        SlowCompress

cdef extern from "../src/Record.hpp" namespace "gtar":
    cdef enum Behavior:
        Constant
        Discrete
        Continuous

    cdef enum Format:
        Float32
        Float64
        Int32
        Int64
        UInt8
        UInt32
        UInt64

    cdef enum Resolution:
        Text
        Uniform
        Individual

    cdef cppclass Record:
        Record()
        Record(const string&)
        Record(const string&, const string&, const string&, const string&,
               Behavior, Format, Resolution)
        Record(const Record&)

        void copy(const Record&)

        string nullifyIndex()
        Record withNullifiedIndex() const

        string getPath() const

        string getIndex() const
        void setIndex(const string&)

cdef extern from "../src/GTAR.hpp" namespace "gtar":
    cdef cppclass GTAR:
        GTAR(const string&, OpenMode)
        void writeString(const string&, const string&, CompressMode)
        void writeBytes(const string&, const vector[char]&, CompressMode)
        void writePtr(const string&, const void*, const size_t, CompressMode)

        SharedArray[char] readBytes(const string&)

        vector[Record] getRecordTypes() const
        vector[string] queryFrames(const Record&) const