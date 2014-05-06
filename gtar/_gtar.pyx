# distutils: language = c++
# distutils: sources = src/Archive.cpp src/vogl_miniz.cpp src/vogl_miniz_zip.cpp src/GTAR.cpp src/Record.cpp

from libcpp.string cimport string
from libcpp.vector cimport vector
from cython.operator cimport dereference as deref
import numpy as np
cimport numpy as np
from cpython cimport PyObject, Py_INCREF

cimport cpp

np.import_array()

cdef class OpenMode:
    """Enum for ways in which an archive file can be opened: Read,
    Write, and Append"""
    Read = cpp.Read
    Write = cpp.Write
    Append = cpp.Append

cdef class CompressMode:
    """Enum for ways in which files within an archive can be
    compressed: NoCompress, FastCompress, MediumCompress, and
    SlowCompress"""
    NoCompress = cpp.NoCompress
    FastCompress = cpp.FastCompress
    MediumCompress = cpp.MediumCompress
    SlowCompress = cpp.SlowCompress

cdef class Behavior:
    """Enum for how properties can behave over time: Constant,
    Discrete, and Continuous"""
    Constant = cpp.Constant
    Discrete = cpp.Discrete
    Continuous = cpp.Continuous

cdef class Format:
    """Formats in which binary properties can be stored: Float{32,
    64}, Int{32, 64}, UInt{8, 32, 64}"""
    Float32 = cpp.Float32
    Float64 = cpp.Float64
    Int32 = cpp.Int32
    Int64 = cpp.Int64
    UInt8 = cpp.UInt8
    UInt32 = cpp.UInt32
    UInt64 = cpp.UInt64

cdef class Resolution:
    """Resolution at which properties can be recorded: Text, Uniform,
    and Individual"""
    Text = cpp.Text
    Uniform = cpp.Uniform
    Individual = cpp.Individual

cdef class SharedArray:
    """Wrapper for the c++ SharedArray<char> class"""
    cdef cpp.SharedArray[char] *thisptr

    def __cinit__(self):
        """Initialize an empty SharedArray object"""
        self.thisptr = new cpp.SharedArray[char]()

    def __dealloc__(self):
        """Deallocate the reference to the currently-held SharedArray
        object"""
        del self.thisptr

    cdef copy(self, const cpp.SharedArray[char] &other):
        """Make this SharedArray point to the given target"""
        self.thisptr.copy(other)

    def __len__(self):
        """Return the number of elements (characters) in this
        SharedArray"""
        return self.thisptr.size()

    def __getitem__(self, index):
        """Access to individual elements in the SharedArray"""
        if(index < self.thisptr.size()):
            return self.thisptr.get()[index]
        else:
            raise IndexError('Index out of range')

    def __array__(self):
        """Interface to create a numpy array from this object"""
        cdef np.npy_intp shape[1]
        shape[0] = <np.npy_intp> self.thisptr.size()
        return np.PyArray_SimpleNewFromData(1, shape,
                                            np.NPY_UINT8, self.thisptr.get())

    def __bytes__(self):
        return self.thisptr.get()[:self.thisptr.size()]

    def __str__(self):
        return self.__bytes__()

    def _setBase(self, arr):
        """Sets the base of arr to be this object and increases the
        reference count"""
        cpp.PyArray_SetBaseObject(arr, self)
        Py_INCREF(self)

    def _arrayRecord(self, rec):
        """Create a numpy array from this object given some metadata
        in a Record object"""
        buf = np.asarray(self)
        self._setBase(buf)

        formats = {cpp.Float32: '<f4',
                   cpp.Float64: '<f8',
                   cpp.Int32: '<i4',
                   cpp.Int64: '<i8',
                   cpp.UInt32: '<u4',
                   cpp.UInt64: '<u8',
                   cpp.UInt8: 'c'}

        widths = {'position': 3,
                  'velocity': 3,
                  'orientation': 4,
                  'image': 3,
                  'body_angular_momentum': 3}

        result = np.frombuffer(buf, dtype=formats[rec.getFormat()])

        name = rec.getName()
        if name in widths:
            result = result.reshape((-1, widths[name]))

        return result

cdef class Record:
    """Python wrapper for the c++ Record class. Provides basic access
    to Record methods."""
    cdef cpp.Record *thisptr

    def __cinit__(self, *args):
        """Initialize a record object in different ways depending on
        the arguments:

        - No arguments: default constructor
        - 1 argument: Parse the given path
        - 7 arguments: Fill each field of the Record object
        """
        if len(args) == 0:
            self.thisptr = new cpp.Record()
        elif len(args) == 1:
            self.thisptr = new cpp.Record(<string> args[0])
        elif len(args) == 7:
            self.thisptr = new cpp.Record(args[0], args[1], args[2], args[3], args[4], args[5], args[6])
        else:
            raise TypeError('Incorrect number of arguments to Record()')

    def __dealloc__(self):
        """Clean up the record allocated for this object"""
        del self.thisptr

    cdef copy(self, const cpp.Record &other):
        """Set this object to be a copy of the given object"""
        self.thisptr.copy(other)

    def nullifyIndex(self):
        """Nullify the index field of this object"""
        return self.thisptr.nullifyIndex()

    def getName(self):
        """Returns the name field of this object"""
        return self.thisptr.getName()

    def getFormat(self):
        """Returns the format field of this object"""
        return self.thisptr.getFormat()

    def getPath(self):
        """Generates the path of the file inside the archive for this object"""
        return self.thisptr.getPath()

    def getIndex(self):
        """Returns the index field for this object"""
        return self.thisptr.getIndex()

    def setIndex(self, index):
        """Sets the index field of this object"""
        self.thisptr.setIndex(index)

cdef class GTAR:
    """Python wrapper for the GTAR c++ class. Provides basic access to
    its methods and simple methods to read and write files within
    archives."""
    cdef cpp.GTAR *thisptr

    def __cinit__(self, path, cpp.OpenMode mode):
        """Initialize a GTAR object given an archive path and open mode"""
        self.thisptr = new cpp.GTAR(path, mode)

    def __dealloc__(self):
        """Destroy the held GTAR object"""
        del self.thisptr

    def __enter__(self):
        """Enter a context with this object"""
        return self

    def __exit__(self, type, value, traceback):
        """Exit a context with this object"""
        self.thisptr.close()

    def writeBytes(self, path, contents, mode=cpp.FastCompress):
        """Write the given contents to the location within the
        archive, using the given compression mode.

        Example:
        >> gtar.writeBytes('params.json', json.dumps(params))
        """
        self.thisptr.writeString(path, contents, mode)

    def readBytes(self, path):
        """Read the contents of the given location within the archive,
        or return None if not found"""
        result = SharedArray()
        result.copy(self.thisptr.readBytes(path))

        return (bytes(result) if len(result) else None)

    def getRecordTypes(self):
        """Returns a python list of all the record types (without
        index information) available in this archive"""
        result = []
        types = self.thisptr.getRecordTypes()
        for rec in types:
            copy = Record()
            copy.copy(rec)
            result.append(copy)
        return result

    def queryFrames(self, Record target):
        """Returns a python list of all indices associated with a
        given record available in this archive"""
        result = []
        frames = self.thisptr.queryFrames(deref(target.thisptr))
        for f in frames:
            result.append(f)
        return result

    def getRecord(self, Record query, string index=""):
        """Returns a numpy array of the contents of the given base
        record and index."""
        rec = Record()
        rec.copy(deref(query.thisptr))
        rec.setIndex(index)

        cdef cpp.SharedArray[char] inter = self.thisptr.readBytes(rec.thisptr.getPath())
        result = SharedArray()
        result.copy(inter)
        return result._arrayRecord(rec)
