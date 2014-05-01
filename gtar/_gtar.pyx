# distutils: language = c++
# distutils: sources = src/Archive.cpp src/vogl_miniz.cpp src/vogl_miniz_zip.cpp src/GTAR.cpp src/Record.cpp

from cython cimport view
from libcpp.string cimport string
from libcpp.vector cimport vector
from cython.operator cimport dereference as deref

cimport cpp

cdef class OpenMode:
    Read = cpp.Read
    Write = cpp.Write
    Append = cpp.Append

cdef class CompressMode:
    NoCompress = cpp.NoCompress
    FastCompress = cpp.FastCompress
    MediumCompress = cpp.MediumCompress
    SlowCompress = cpp.SlowCompress

cdef class Behavior:
    Constant = cpp.Constant
    Discrete = cpp.Discrete
    Continuous = cpp.Continuous

cdef class Format:
    Float32 = cpp.Float32
    Float64 = cpp.Float64
    Int32 = cpp.Int32
    Int64 = cpp.Int64
    UInt8 = cpp.UInt8
    UInt32 = cpp.UInt32
    UInt64 = cpp.UInt64

cdef class Resolution:
    Text = cpp.Text
    Uniform = cpp.Uniform
    Individual = cpp.Individual

cdef class SharedArray:
    cdef cpp.SharedArray[char] *thisptr

    def __cinit__(self):
        self.thisptr = new cpp.SharedArray[char]()

    def __dealloc__(self):
        del self.thisptr

    cdef copy(self, const cpp.SharedArray[char] &other):
        self.thisptr.copy(other)

    def __len__(self):
        return self.thisptr.size()

    def __getitem__(self, index):
        if(index < self.thisptr.size()):
            return self.thisptr.get()[index]
        else:
            raise IndexError('Index out of range')

cdef class Record:
    cdef cpp.Record *thisptr

    def __cinit__(self, *args):
        if len(args) == 0:
            self.thisptr = new cpp.Record()
        elif len(args) == 1:
            self.thisptr = new cpp.Record(<string> args[0])
        elif len(args) == 7:
            self.thisptr = new cpp.Record(args[0], args[1], args[2], args[3], args[4], args[5], args[6])
        else:
            raise TypeError('Incorrect number of arguments to Record()')

    def __dealloc__(self):
        del self.thisptr

    cdef copy(self, const cpp.Record &other):
        self.thisptr.copy(other)

    def nullifyIndex(self):
        return self.thisptr.nullifyIndex()

    def getPath(self):
        return self.thisptr.getPath()

    def getIndex(self):
        return self.thisptr.getIndex()

    def setIndex(self, index):
        self.thisptr.setIndex(index)

cdef class GTAR:
    cdef cpp.GTAR *thisptr

    def __cinit__(self, path, cpp.OpenMode mode):
        self.thisptr = new cpp.GTAR(path, mode)

    def __dealloc__(self):
        del self.thisptr

    def writeBytes(self, path, contents, mode=cpp.FastCompress):
        self.thisptr.writeString(path, contents, mode)

    def getRecordTypes(self):
        result = []
        types = self.thisptr.getRecordTypes()
        for rec in types:
            copy = Record()
            copy.copy(rec)
            result.append(copy)
        return result

    def queryFrames(self, Record target):
        result = []
        frames = self.thisptr.queryFrames(deref(target.thisptr))
        for f in frames:
            result.append(f)
        return result

    def getRecord(self, Record query, string index=""):
        rec = Record()
        rec.copy(deref(query.thisptr))
        rec.setIndex(index)

        cdef cpp.SharedArray[char] inter = self.thisptr.readBytes(rec.thisptr.getPath())
        cdef unsigned int n = inter.size()
        cdef view.array result = <char[:n]> inter.get()

        # cdef array[char] result = array('c')
        # cdef unsigned int n = inter.size()
        # cdef view.array arr = <char[:n]> inter.get()
        # result = clone(arr, inter.size(), False)

        # result = SharedArray()
        # print(rec.thisptr.getPath())
        # result.copy(self.thisptr.readBytes(rec.thisptr.getPath()))
        return result.copy()