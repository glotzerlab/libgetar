# distutils: language = c++
# distutils: sources = src/Archive.cpp src/vogl_miniz.cpp src/vogl_miniz_zip.cpp src/GTAR.cpp src/Record.cpp

from libcpp.string cimport string
from libcpp.vector cimport vector
from cython.operator cimport dereference as deref

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

cdef class PyRecord:
    cdef Record *thisptr

    def __cinit__(self, *args):
        if len(args) == 0:
            self.thisptr = new Record()
        elif len(args) == 1:
            self.thisptr = new Record(<string> args[0])
        elif len(args) == 7:
            self.thisptr = new Record(args[0], args[1], args[2], args[3], args[4], args[5], args[6])
        else:
            raise TypeError('Incorrect number of arguments to PyRecord()')

    def __dealloc__(self):
        del self.thisptr

    cdef copy(self, const Record &other):
        self.thisptr.copy(other)

    def nullifyIndex(self):
        return self.thisptr.nullifyIndex()

    def getPath(self):
        return self.thisptr.getPath()

    def getIndex(self):
        return self.thisptr.getIndex()

    def setIndex(self, index):
        self.thisptr.setIndex(index)

cdef class PyGTAR:
    cdef GTAR *thisptr

    def __cinit__(self, path, OpenMode mode):
        self.thisptr = new GTAR(path, mode)

    def __dealloc__(self):
        del self.thisptr

    def writeBytes(self, path, contents, mode=FastCompress):
        self.thisptr.writeString(path, contents, mode)

    def getRecordTypes(self):
        result = []
        types = self.thisptr.getRecordTypes()
        for rec in types:
            copy = PyRecord()
            copy.copy(rec)
            result.append(copy)
        return result

    def queryFrames(self, PyRecord target):
        result = []
        frames = self.thisptr.queryFrames(deref(target.thisptr))
        for f in frames:
            result.append(f)
        return result