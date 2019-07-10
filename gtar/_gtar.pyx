# distutils: language = c++
# cython: embedsignature=True

from libcpp.string cimport string
from libcpp.vector cimport vector
from cython.operator cimport dereference as deref
import numpy as np
cimport numpy as np
from cpython cimport PyObject, Py_INCREF

cimport cpp
from cpp cimport GTAR as GTAR_

np.import_array()

cdef class OpenMode:
    """
    Enum for ways in which an archive file can be opened

       .. data:: Read
       .. data:: Write
       .. data:: Append
    """
    Read = cpp.Read
    Write = cpp.Write
    Append = cpp.Append

cdef class CompressMode:
    """
    Enum for ways in which files within an archive can be compressed

       .. data:: NoCompress
       .. data:: FastCompress
       .. data:: MediumCompress
       .. data:: SlowCompress"""
    NoCompress = cpp.NoCompress
    FastCompress = cpp.FastCompress
    MediumCompress = cpp.MediumCompress
    SlowCompress = cpp.SlowCompress

cdef class Behavior:
    """
    Enum for how properties can behave over time

       .. data:: Constant
       .. data:: Discrete
       .. data:: Continuous"""
    Constant = cpp.Constant
    Discrete = cpp.Discrete
    Continuous = cpp.Continuous

cdef class Format:
    """
    Formats in which binary properties can be stored

       .. data:: Float32
       .. data:: Float64
       .. data:: Int32
       .. data:: Int64
       .. data:: UInt8
       .. data:: UInt32
       .. data:: UInt64"""
    Float32 = cpp.Float32
    Float64 = cpp.Float64
    Int32 = cpp.Int32
    Int64 = cpp.Int64
    UInt8 = cpp.UInt8
    UInt32 = cpp.UInt32
    UInt64 = cpp.UInt64

cdef class Resolution:
    """
    Resolution at which properties can be recorded

       .. data:: Text
       .. data:: Uniform
       .. data:: Individual"""
    Text = cpp.Text
    Uniform = cpp.Uniform
    Individual = cpp.Individual

cdef string py3str(arg):
    return <string> arg.encode('utf8')

cdef unpy3str(arg):
    return arg.decode('utf8')

widths = {'position': 3,
          'velocity': 3,
          'orientation': 4,
          'image': 3,
          'angular_momentum': 3,
          'angular_momentum_quat': 4,
          'moment_inertia': 3,
          'virial': 6,
          'center_of_mass': 3}

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
        result = np.PyArray_SimpleNewFromData(1, shape,
                                            np.NPY_UINT8, self.thisptr.get())
        self._setBase(result)

        return result

    def __bytes__(self):
        return <bytes> self.thisptr.get()[:self.thisptr.size()]

    def __str__(self):
        return unpy3str(self.__bytes__())

    def _setBase(self, arr):
        """Sets the base of arr to be this object and increases the
        reference count"""
        cpp.PyArray_SetBaseObject(arr, self)
        Py_INCREF(self)

    def _arrayRecord(self, rec):
        """Create a numpy array from this object given some metadata in a Record
        object. Uses the widths dictionary in the gtar namespace to
        reshape the array into an Nxwidths[prop] array if the property's
        name is present there."""
        buf = np.asarray(self)

        formats = {cpp.Float32: '<f4',
                   cpp.Float64: '<f8',
                   cpp.Int32: '<i4',
                   cpp.Int64: '<i8',
                   cpp.UInt32: '<u4',
                   cpp.UInt64: '<u8',
                   cpp.UInt8: 'c'}

        result = np.frombuffer(buf, dtype=formats[rec.getFormat()])

        name = rec.getName()
        if name in widths:
            result = result.reshape((-1, widths[name]))

        return result

cdef class Record:
    """Python wrapper for the c++ Record class. Provides basic access to
    Record methods. Initializes in different ways depending on the
    number of given parameters.

    - No arguments: default constructor
    - 1 argument: Parse the given path
    - 6 arguments: Fill each field of the Record object (*group*, *name*, *index*, *behavior*, *format*, *resolution*)
    """

    cdef cpp.Record *thisptr

    def __cinit__(self, *args):
        if len(args) == 0:
            self.thisptr = new cpp.Record()
        elif len(args) == 1:
            self.thisptr = new cpp.Record(py3str(args[0]))
        elif len(args) == 6:
            self.thisptr = new cpp.Record(py3str(args[0]), py3str(args[1]),
                                          py3str(args[2]),
                                          args[3], args[4], args[5])
        else:
            raise TypeError('Incorrect number of arguments to Record()')

    def __dealloc__(self):
        """Clean up the record allocated for this object"""
        del self.thisptr

    def __richcmp__(self, other, int op):
        self_vals = [self.getGroup(), self.getName(), self.getIndex(),
                     self.getBehavior(), self.getFormat(), self.getResolution()]
        other_vals = [self.getGroup(), self.getName(), self.getIndex(),
                     self.getBehavior(), self.getFormat(), self.getResolution()]
        if op == 0:
            return self_vals < other_vals
        elif op == 1:
            return self_vals <= other_vals
        elif op == 2:
            return self_vals == other_vals
        elif op == 3:
            return self_vals != other_vals
        elif op == 4:
            return self_vals > other_vals
        elif op == 5:
            return self_vals >= other_vals
        else:
            raise TypeError('Unknown operator {} in __richcmp__'.format(op))

    def __hash__(self):
        return hash((self.__class__, repr(self)))

    def __reduce__(self):
        return (self.__class__, (self.getPath(),))

    def __repr__(self):
        """Returns a string representation of this object"""
        return 'Record("{}")'.format(self.getPath())

    cdef copy(self, const cpp.Record &other):
        """Set this object to be a copy of the given object"""
        self.thisptr.copy(other)

    def nullifyIndex(self):
        """Nullify the index field of this object"""
        return unpy3str(self.thisptr.nullifyIndex())

    def getBehavior(self):
        """Returns the `behavior` field of this object"""
        return self.thisptr.getBehavior()

    def getGroup(self):
        """Returns the `group` field of this object"""
        return unpy3str(self.thisptr.getGroup())

    def getName(self):
        """Returns the `name` field of this object"""
        return unpy3str(self.thisptr.getName())

    def getFormat(self):
        """Returns the `format` field of this object"""
        return self.thisptr.getFormat()

    def getPath(self):
        """Generates the path of the file inside the archive for this object"""
        return unpy3str(self.thisptr.getPath())

    def getResolution(self):
        """Returns the `resolution` field for this object"""
        return self.thisptr.getResolution()

    def getIndex(self):
        """Returns the `index` field for this object"""
        return unpy3str(self.thisptr.getIndex())

    def setIndex(self, index):
        """Sets the `index` field of this object"""
        self.thisptr.setIndex(py3str(index))

cdef class BulkWriter:
    """Class for efficiently writing multiple records at a time. Works
    as a context manager.

    :param arch: :py:class:`gtar.GTAR` archive object to write within

    Example::

        with gtar.GTAR('traj.sqlite', 'w') as traj, traj.getBulkWriter() as writer:
            writer.writeStr('notes.txt', 'example text')

    """
    cdef GTAR_.BulkWriter *thisptr

    def __cinit__(self, GTAR arch):
        self.thisptr = new GTAR_.BulkWriter(deref(arch.thisptr))

    cdef _dealloc(self):
        del self.thisptr
        self.thisptr = <GTAR_.BulkWriter*> 0

    def __dealloc__(self):
        self._dealloc()

    def __enter__(self):
        return self

    def __exit__(self, type, value, traceback):
        self._dealloc()

    def writeBytes(self, path, contents, mode=cpp.FastCompress):
        """Write the given contents to the location within the
        archive, using the given compression mode.

        :param path: Path within the archive to write
        :param contents: Bytestring to write
        :param mode: Optional compression mode (defaults to fast compression)
        """
        self.thisptr.writeString(py3str(path), contents, mode)

    def writeStr(self, path, contents, mode=cpp.FastCompress):
        """Write the given string to the given path, optionally
        compressing with the given mode.

        :param path: Path within the archive to write
        :param contents: String to write
        :param mode: Optional compression mode (defaults to fast compression)

        Example::

            writer.writeStr('params.json', json.dumps(params))
        """
        self.writeBytes(path, contents.encode('utf-8'), mode)

    def writePath(self, path, contents, mode=cpp.FastCompress):
        """Writes the given contents to the given path, converting as
        necessary.

        :param path: Path within the archive to write
        :param contents: Object which can be converted into array or string form, based on the given path
        :param mode: Optional compression mode (defaults to fast compression)
        """
        rec = Record(path)
        self.writeRecord(rec, contents, mode)

    def writeArray(self, path, arr, mode=cpp.FastCompress, dtype=None):
        """Write the given numpy array to the location within the
        archive, using the given compression mode. This serializes the
        data into the given binary data type or the same binary format
        that the numpy array is using.

        :param path: Path within the archive to write
        :param arr: Array-like object
        :param mode: Optional compression mode (defaults to fast compression)
        :param dtype: Optional numpy dtype to force conversion to

        Example::

            writer.writeArray('diameter.f32.ind', numpy.ones((N,)))
        """
        arr = np.ascontiguousarray(np.asarray(arr).flat, dtype=dtype)
        cdef np.ndarray[char, ndim=1, mode="c"] carr = np.frombuffer(arr, dtype=np.uint8)
        if carr.nbytes:
            self.thisptr.writePtr(py3str(path), &carr[0], carr.nbytes, mode)
        else:
            self.thisptr.writePtr(py3str(path), <void*> 0, carr.nbytes, mode)

    def writeRecord(self, Record rec, contents, mode=cpp.FastCompress):
        """Writes the given contents to the path specified by the given record.

        :param rec: :py:class:`gtar.Record` object specifying the record
        :param contents: [byte]string or array-like object to write
        :param mode: Optional compression mode (defaults to fast compression)
        """
        dtypes = {cpp.Float32: np.float32,
                  cpp.Float64: np.float64,
                  cpp.Int32: np.int32,
                  cpp.Int64: np.int64,
                  cpp.UInt8: np.uint8,
                  cpp.UInt32: np.uint32,
                  cpp.UInt64: np.uint64}

        if rec.getResolution() == cpp.Text:
            if type(contents) == str:
                self.writeStr(rec.getPath(), contents, mode)
            elif type(contents) == bytes:
                self.writeBytes(rec.getPath(), contents, mode)
            else:
                raise RuntimeError('Not sure how to serialize the given argument!')
        else:
            self.writeArray(rec.getPath(), contents, mode,
                            dtype=dtypes[rec.getFormat()])

cdef class GTAR:
    """Python wrapper for the :cpp:class:`GTAR` c++ class. Provides
    basic access to its methods and simple methods to read and write
    files within archives.

    The backend is automatically selected based on the suffix of the
    given path: if the name ends in '.tar', a tar-format archive will
    be created, if it ends in '.sqlite' a sqlite-format archive will
    be created, if it ends in '/' a directory structure (filesystem)
    "archive" will be created, otherwise a zip-format archive will be
    created.

    The open mode controls how the file will be opened.

    - read: The file will be opened in read-only mode
    - write: A new file will be opened for writing, potentially overwriting an existing file of the same name
    - append: A file will be opened for writing, adding to the end of a file if it already exists with the same name

    :param path: Path to the file to open
    :param mode: Open mode: one of 'r', 'w', 'a'
    """
    cdef cpp.GTAR *thisptr
    cdef _path
    cdef _mode

    openModes = {'r': cpp.Read,
                 'w': cpp.Write,
                 'a': cpp.Append}

    def __cinit__(self, path, mode):
        """Initialize a `GTAR` object given an archive path and open mode"""
        self._path = path
        self._mode = mode
        try:
            self.thisptr = new cpp.GTAR(py3str(path), self.openModes[self._mode])
        except KeyError:
            raise RuntimeError('Unknown open mode: {}'.format(self._mode))
        except RuntimeError as e:
            raise RuntimeError('{} for file {}'.format(', '.join(e.args), path))

    def __dealloc__(self):
        """Destroy the held `GTAR` object"""
        del self.thisptr

    def __enter__(self):
        """Enter a context with this object"""
        return self

    def __exit__(self, type, value, traceback):
        """Exit a context with this object"""
        self.close()

    def __reduce__(self):
        return (self.__class__, (self._path, self._mode))

    def _sortFrameKey(self, v):
        """Key function for sorting frame indices"""
        return (len(v), v)

    def close(self):
        """Close the file this object is writing to. It is safe to
        close a file multiple times, but impossible to read from or
        write to it after closing."""
        self.thisptr.close()

    def readBytes(self, path):
        """Read the contents of the given location within the archive,
        or return ``None`` if not found

        :param path: Path within the archive to write
        """
        result = SharedArray()
        result.copy(self.thisptr.readBytes(py3str(path)))

        return (bytes(result) if len(result) else None)

    def writeBytes(self, path, contents, mode=cpp.FastCompress):
        """Write the given contents to the location within the
        archive, using the given compression mode.

        :param path: Path within the archive to write
        :param contents: Bytestring to write
        :param mode: Optional compression mode (defaults to fast compression)
        """
        self.thisptr.writeString(py3str(path), contents, mode)

    def readStr(self, path):
        """Read the contents of the given path as a string or return
        ``None`` if not found.

        :param path: Path within the archive to write
        """
        result = self.readBytes(path)
        if result is not None:
            return result.decode('utf8')
        else:
            return result

    def writeStr(self, path, contents, mode=cpp.FastCompress):
        """Write the given string to the given path, optionally
        compressing with the given mode.

        :param path: Path within the archive to write
        :param contents: String to write
        :param mode: Optional compression mode (defaults to fast compression)

        Example::

            gtar.writeStr('params.json', json.dumps(params))
        """
        self.writeBytes(path, contents.encode('utf-8'), mode)

    def readPath(self, path):
        """Reads the contents of a record at the given path. Returns ``None`` if
        not found. If an array is found and the property is present in
        ``gtar.widths``, reshape into an Nxwidths[prop] array.

        :param path: Path within the archive to write
        """
        rec = Record(path)
        return self.getRecord(rec, rec.getIndex())

    def writePath(self, path, contents, mode=cpp.FastCompress):
        """Writes the given contents to the given path, converting as
        necessary.

        :param path: Path within the archive to write
        :param contents: Object which can be converted into array or string form, based on the given path
        :param mode: Optional compression mode (defaults to fast compression)
        """
        rec = Record(path)
        self.writeRecord(rec, contents, mode)

    def writeArray(self, path, arr, mode=cpp.FastCompress, dtype=None):
        """Write the given numpy array to the location within the
        archive, using the given compression mode. This serializes the
        data into the given binary data type or the same binary format
        that the numpy array is using.

        :param path: Path within the archive to write
        :param arr: Array-like object
        :param mode: Optional compression mode (defaults to fast compression)
        :param dtype: Optional numpy dtype to force conversion to

        Example::

            gtar.writeArray('diameter.f32.ind', numpy.ones((N,)))
        """
        arr = np.ascontiguousarray(np.asarray(arr).flat, dtype=dtype)
        cdef np.ndarray[char, ndim=1, mode="c"] carr = np.frombuffer(arr, dtype=np.uint8)
        if carr.nbytes:
            self.thisptr.writePtr(py3str(path), &carr[0], carr.nbytes, mode)
        else:
            self.thisptr.writePtr(py3str(path), <void*> 0, carr.nbytes, mode)

    def getBulkWriter(self):
        """Get a :py:class:`gtar.BulkWriter` context object. These allow for more
        efficient writes when writing many records at once."""
        return BulkWriter(self)

    def getRecord(self, Record query, index=""):
        """Returns the contents of the given base record and index.

        :param query: Prototypical :py:class:`gtar.Record` object describing the record to fetch
        :param index: Index used to fetch the record (defaults to index embedded in :code:`query`)
        :type query: :py:class:`gtar.Record`
        :type index: string

        .. note::

           If an index is passed into this function, it takes precedence over the index embedded in the given record.

        """
        rec = Record()
        rec.copy(deref(query.thisptr))

        if index != "":
            rec.setIndex(index)

        cdef cpp.SharedArray[char] inter
        inter = self.thisptr.readBytes(rec.thisptr.getPath())
        result = SharedArray()
        result.copy(inter)

        if rec.thisptr.getResolution() != cpp.Text:
            return result._arrayRecord(rec)
        else:
            try:
                return str(result)
            except UnicodeDecodeError:
                return bytes(result)

    def writeRecord(self, Record rec, contents, mode=cpp.FastCompress):
        """Writes the given contents to the path specified by the given record.

        :param rec: :py:class:`gtar.Record` object specifying the record
        :param contents: [byte]string or array-like object to write
        :param mode: Optional compression mode (defaults to fast compression)
        """
        dtypes = {cpp.Float32: np.float32,
                  cpp.Float64: np.float64,
                  cpp.Int32: np.int32,
                  cpp.Int64: np.int64,
                  cpp.UInt8: np.uint8,
                  cpp.UInt32: np.uint32,
                  cpp.UInt64: np.uint64}

        if rec.getResolution() == cpp.Text:
            if type(contents) == str:
                self.writeStr(rec.getPath(), contents, mode)
            elif type(contents) == bytes:
                self.writeBytes(rec.getPath(), contents, mode)
            else:
                raise RuntimeError('Not sure how to serialize the given argument!')
        else:
            self.writeArray(rec.getPath(), contents, mode,
                            dtype=dtypes[rec.getFormat()])

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
        given record available in this archive

        :param target: Prototypical :py:class:`gtar.Record` object (the index of which is unused)
        """
        result = []
        frames = self.thisptr.queryFrames(deref(target.thisptr))
        for f in frames:
            result.append(unpy3str(f))
        return result

    def framesWithRecordsNamed(self, names):
        """Returns ``([record(val) for val in names], [frames])`` given a
        set of record names names. If only given a single name,
        returns ``(record, [frames])``.

        :param names: Iterable object yielding a set of property names
        """
        allRecords = dict((rec.getName(), rec) for rec in self.getRecordTypes())
        frames = None
        records = []

        if type(names) == type(''):
            names = [names]

        for n in names:
            try:
                rec = allRecords[n]
                records.append(rec)
            except KeyError:
                return (None, [])

            f = self.queryFrames(rec)

            if frames is not None:
                frames.intersection_update(f)
            else:
                frames = set(f)

        if len(names) > 1:
            return (records, sorted(frames, key=self._sortFrameKey))
        else:
            return (records[0], sorted(frames, key=self._sortFrameKey))

    def recordsNamed(self, names):
        """Returns ``(frame, [val[frame] for val in names])`` for each frame which
        contains records matching each of the given names. If only given
        a single name, returns ``(frame, val[frame])`` for each found
        frame. If a property is present in ``gtar.widths``, returns it
        as an Nxwidths[prop] array.

        :param names: Iterable object yielding a set of property names

        Example::

            g = gtar.GTAR('dump.zip', 'r')

            # grab single property
            for (_, vel) in g.recordsNamed('velocity'):
                pass

            # grab multiple properties
            for (idx, (pos, quat)) in g.recordsNamed(['position', 'orientation']):
                pass
        """
        allRecords = dict((rec.getName(), rec) for rec in self.getRecordTypes())
        frames = None

        if type(names) == type(''):
            names = [names]

        for n in names:
            try:
                rec = allRecords[n]
            except KeyError:
                return []
                # raise KeyError('Can\'t find a record named {}'.format(n))

            f = self.queryFrames(rec)

            if frames is not None:
                frames.intersection_update(f)
            else:
                frames = set(f)

        if len(names) > 1:
            for frame in sorted(frames, key=self._sortFrameKey):
                yield frame, tuple(self.getRecord(allRecords[n], frame) for n in names)
        else:
            for frame in sorted(frames, key=self._sortFrameKey):
                yield frame, self.getRecord(allRecords[names[0]], frame)

    def staticRecordNamed(self, name):
        """Returns a static record with the given name. If the property is found in
        ``gtar.widths``, returns it as an Nxwidths[prop] array.

        :param name: Name of the property to find
        """
        try:
            rec = [rec for rec in self.getRecordTypes() if rec.getName() == name][0]
            return self.getRecord(rec)
        except IndexError:
            raise KeyError('Can\'t find a static record named {}'.format(name))

def isZip64(filename):
    """Internal helper function. Returns ``True`` if a file located at the
    given path is in zip64 format."""
    return cpp.isZip64(py3str(filename))
