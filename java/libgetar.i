
%module libgetar_wrap
%{
#include "../src/GTAR.hpp"
using namespace gtar;
%}

%clear char*;

/// Generic reference-counting shared array implementation for
/// arbitrary datatypes.
template<typename T>
class SharedArray
{
public:
    typedef T* iterator;

    /// Default constructor. Allocates nothing.
    SharedArray();
    /// Target constructor: allocates a new SharedArrayShim for the
    /// given pointer and takes ownership of it.
    SharedArray(T *target, size_t length);
    /// Copy constructor: make this object point to the same array as
    /// rhs, increasing the reference count if necessary
    SharedArray(const SharedArray<T> &rhs);
    /// Destructor: decrement the reference count and deallocate if we
    /// are the last owner of the pointer
    ~SharedArray();
    /// Non-operator form of assignment
    void copy(const SharedArray<T> &rhs);
    /// Returns true if m_shim is null or m_shim's target is null
    bool isNull();
    /// Assignment operator: make this object point to the same thing
    /// as rhs (and deallocate our old memory if necessary)
    void operator=(const SharedArray<T> &rhs);
    /// Returns a standard style iterator to the start of the array
    iterator begin();
    /// Returns a standard style iterator to just past the end of the array
    iterator end();
    /// Returns the raw pointer held (NULL otherwise)
    T *get();
    /// Returns the size, in number of objects, of this array
    size_t size() const;

    /// Release our claim on the pointer, including decrementing the
    /// reference count
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

    /// Swap the contents of this array with another
    void swap(SharedArray<T> &target);

    /// Access elements by index
    T &operator[](size_t idx);

    /// Const access to elements by index
    const T &operator[](size_t idx) const;
};


%template(SharedArrayChar) SharedArray<char>;

%typemap(jni) SharedArray<char> "jbyteArray"
%typemap(jtype) SharedArray<char> "byte[]"
%typemap(jstype) SharedArray<char> "byte[]"
%typemap(javaout) SharedArray<char> {
    return $jnicall;
}

%typemap(out) SharedArray<char> {
    $result = JCALL1(NewByteArray, jenv, $1.size());
    JCALL4(SetByteArrayRegion, jenv, $result, 0, $1.size(), (const jbyte*) $1.get());
}

%include "std_string.i" // for std::string type-maps
#include <string>
using std::string;
%include "std_vector.i" // for std::vector type-maps
#include <vector>
using std::vector;

%template(VectorRecords) std::vector<Record>;
%template(VectorStrings) std::vector<string>;

/// Accessor interface for a trajectory archive
class GTAR
{
public:
    typedef set<string, IndexCompare> indexSet;

    /// Constructor. Opens the zip file at filename in the given
    /// mode.
    GTAR(const string &filename, const OpenMode mode);

    /// Manually close the opened archive (it automatically closes
    /// itself upon destruction)
    void close();

    /// Read a bytestring from the specified location
    SharedArray<char> readBytes(const string &path);

    /// Query all of the records in the archive. These will all
    /// have empty indices.
    std::vector<Record> getRecordTypes() const;
    /// Query the indices associated with a given record. The
    /// record is not required to have a null index.
    std::vector<string> queryFrames(const Record &target) const;
};

/// Time behavior of properties
enum Behavior {Constant, Discrete, Continuous};

/// Binary formats in which properties can be stored
enum Format {Float32, Float64, Int32, Int64, UInt8, UInt32, UInt64};

/// Level of detail of property storage
enum Resolution {Text, Uniform, Individual};

/// Simple class for a record which can be stored in an archive
class Record
{
public:
    /// Default constructor: initialize all strings to empty,
    /// behavior to Constant, format to UInt8, and resolution to
    /// Text
    Record();

    /// Create a record from a path (inside the archive), parsing
    /// the path into the various fields
    Record(const string &path);

    /// Create a record directly from the full set of elements
    Record(const string &group, const string &name, const string &index,
               Behavior behavior, Format format, Resolution resolution);

    /// Copy constructor
    Record(const Record &rhs);

    /// Assignment operator
    void operator=(const Record &rhs);

    /// Equality
    bool operator==(const Record &rhs) const;
    /// Inequality
    bool operator!=(const Record &rhs) const;
    /// Comparison
    bool operator<(const Record &rhs) const;

    /// Copy all fields from rhs into this object
    void copy(const Record &rhs);

    /// Set our index to the empty string
    string nullifyIndex();
    /// Return a copy of this object, but with an empty string for
    /// its index
    Record withNullifiedIndex() const;

    /// Construct a path (for inside an archive) from this object's
    /// various fields
    string getPath() const;

    /// Get the stored group field
    string getGroup() const;
    /// Get the stored name field
    string getName() const;
    /// Get the stored index field
    string getIndex() const;
    /// Get the stored behavior field
    Behavior getBehavior() const;
    /// Get the stored format field
    Format getFormat() const;
    /// Get the stored resolution field
    Resolution getResolution() const;

    /// Set the index field for this Record object
    void setIndex(const string &index);
};

// Modes in which we can open a file
enum OpenMode {Read, Write, Append};

// Varying degrees to which files can be compressed
enum CompressMode {NoCompress, FastCompress, MediumCompress, SlowCompress};
