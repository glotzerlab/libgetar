# distutils: language = c++
# distutils: sources = src/Archive.cpp src/vogl_miniz.cpp src/vogl_miniz_zip.cpp src/GTAR.cpp src/Record.cpp

from libcpp.string cimport string
from libcpp.vector cimport vector

from sharedarray cimport *

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
        Record(const string&)
        Record(const string&, const string&, const string&, const string&,
               Behavior, Format, Resolution)
        Record(const Record&)

        Record withNullifiedIndex() const

        string getPath() const

        void setIndex(const string&)

cdef extern from "../src/GTAR.hpp" namespace "gtar":
    cdef cppclass GTAR:
        GTAR(const string&, OpenMode)
        void writeString(const string&, const string&, CompressMode)
        void writeBytes(const string&, const vector[char]&, CompressMode)
        void writePtr(const string&, const void*, const size_t, CompressMode)

        SharedArray[char] readBytes(const string&)

cdef class PyOpenMode:
    PyRead = Read
    PyWrite = Write
    PyAppend = Append

cdef class PyCompressMode:
    PyNoCompress = NoCompress
    PyFastCompress = FastCompress
    PyMediumCompress = MediumCompress
    PySlowCompress = SlowCompress

cdef class PyBehavior:
    PyConstant = Constant
    PyDiscrete = Discrete
    PyContinuous = Continuous

cdef class PyFormat:
    PyFloat32 = Float32
    PyFloat64 = Float64
    PyInt32 = Int32
    PyInt64 = Int64
    PyUInt8 = UInt8
    PyUInt32 = UInt32
    PyUInt64 = UInt64

cdef class PyResolution:
    PyText = Text
    PyUniform = Uniform
    PyIndividual = Individual

cdef class PyGTAR:
    cdef GTAR *thisptr

    def __cinit__(self, path, OpenMode mode):
        self.thisptr = new GTAR(path, mode)

    def __dealloc__(self):
        del self.thisptr

    def writeBytes(self, path, contents, mode=FastCompress):
        self.thisptr.writeString(path, contents, mode)