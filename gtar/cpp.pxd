# distutils: language = c++

from libcpp.string cimport string
from libcpp.vector cimport vector
from cython.operator cimport dereference as deref

cimport numpy
from libcpp cimport bool

cdef extern from "numpy/arrayobject.h":
    cdef int PyArray_SetBaseObject(numpy.ndarray arr, obj)

cdef extern from "../src/SharedArray.hpp" namespace "gtar_pymodule::gtar":
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

cdef extern from "../src/Archive.hpp" namespace "gtar_pymodule::gtar":
    cdef enum OpenMode:
        Read
        Write
        Append

    cdef enum CompressMode:
        NoCompress
        FastCompress
        MediumCompress
        SlowCompress

cdef extern from "../src/Record.hpp" namespace "gtar_pymodule::gtar":
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
        Record(const string&, const string&, const string&,
               Behavior, Format, Resolution)
        Record(const Record&)

        void copy(const Record&)

        string nullifyIndex()
        Record withNullifiedIndex() const

        string getPath() const

        Behavior getBehavior() const
        string getGroup() const
        string getName() const
        string getIndex() const
        Format getFormat() const
        Resolution getResolution() const
        void setIndex(const string&)

cdef extern from "../src/GTAR.hpp" namespace "gtar_pymodule::gtar":
    cdef cppclass GTAR:
        cppclass BulkWriter:
            BulkWriter(GTAR&)

            void writeString(const string&, const string&, CompressMode) except +
            void writeBytes(const string&, const vector[char]&, CompressMode) except +
            void writePtr(const string&, const void*, const size_t, CompressMode) except +

        GTAR(const string&, OpenMode) except +

        void close()
        void writeString(const string&, const string&, CompressMode) except +
        void writeBytes(const string&, const vector[char]&, CompressMode) except +
        void writePtr(const string&, const void*, const size_t, CompressMode) except +

        # SharedArray[T] readIndividual[T](const string&)
        SharedArray[char] readBytes(const string&) except +

        vector[Record] getRecordTypes() const
        vector[string] queryFrames(const Record&) const

cdef extern from "../src/ZipArchive.hpp" namespace "gtar_pymodule::gtar":
     bool isZip64(const string&) except +
