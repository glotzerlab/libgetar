// GTAR.hpp
// by Matthew Spellings <mspells@umich.edu>

#include <algorithm>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

#include "Archive.hpp"
#include "SharedArray.hpp"
#include "Record.hpp"

#ifndef __GTAR_HPP_
#define __GTAR_HPP_

// TODO find a proper compile-time way to determine endianness
// #define IS_BIG_ENDIAN (1 != *(unsigned char *)&(const int){1})
#define IS_BIG_ENDIAN false

namespace gtar{

    using std::map;
    using std::runtime_error;
    using std::set;
    using std::string;
    using std::swap;
    using std::vector;

    // Swap the endianness of all elements, if necessary on this
    // architecture
    template<typename T>
    void maybeSwapEndian(char *target, size_t byteLength);

    // Special sorter for index strings such that shorter strings
    // appear first
    class IndexCompare
    {
    public:
        bool operator()(const string &a, const string &b);
    };

    // Accessor interface for a trajectory archive
    class GTAR
    {
    public:
        typedef set<string, IndexCompare> indexSet;

        // Constructor. Opens the zip file at filename in the given
        // mode.
        GTAR(const string &filename, const OpenMode mode);

        // Most generic functions. Toss some bytes into a location,
        // overloaded for different sources.
        void writeString(const string &path, const string &contents, CompressMode mode);
        void writeBytes(const string &path, const vector<char> &contents, CompressMode mode);
        void writePtr(const string &path, const void *contents,
                      const size_t byteLength, CompressMode mode);

        // Write individual and uniform binary properties to the specified
        // location, converting to little endian if necessary.
        template<typename iter, typename T>
        void writeIndividual(const string &path, const iter &start,
                             const iter &end, CompressMode mode);
        template<typename T>
        void writeUniform(const string &path, const T &val);

        // Read individual and uniform binary properties (or just a
        // stream of bytes) from the specified location, converting
        // from little endian if necessary.
        template<typename T>
        SharedArray<T> readIndividual(const string &path);
        template<typename T>
        T readUniform(const string &path);
        SharedArray<char> readBytes(const string &path);

        // Query all of the records in the archive. These will all
        // have empty indices.
        vector<Record> getRecordTypes() const;
        // Query the indices associated with a given record. The
        // record is not required to have a null index.
        vector<string> queryFrames(const Record &target) const;

    private:
        // Insert a record into the set of cached records
        void insertRecord(const string &path);

        // The archive abstraction object we'll use
        Archive m_archive;

        // Cached record objects
        map<Record, indexSet> m_records;
        map<Record, vector<string> > m_indexedRecords;
    };

    // Swap the bytes of a series of characters if this is a big-endian machine
    template<typename T>
    void maybeSwapEndian(T *target, size_t byteLength)
    {
        char *recast((char*) target);

        if(byteLength % sizeof(T) != 0)
            throw runtime_error("Trying to convert an incorrect number of bytes to little-endian");

        if(IS_BIG_ENDIAN)
        {
            for(size_t i(0); i < byteLength/sizeof(T); ++i)
            {
                for(size_t j(0); j < sizeof(T)/2; ++j)
                    swap(recast[i*sizeof(T) + j], recast[(i + 1)*sizeof(T) - j - 1]);
            }
        }
    }

    template<typename iter, typename T>
    void GTAR::writeIndividual(const string &path, const iter &start,
                               const iter &end, CompressMode mode)
    {
        vector<T> buffer(start, end);

        maybeSwapEndian<T>(&buffer[0], buffer.size()*sizeof(T));
        writePtr(path, &buffer[0], buffer.size()*sizeof(T), mode);
    }

    template<typename T>
    void GTAR::writeUniform(const string &path, const T &val)
    {
        T local(val);

        maybeSwapEndian<T>(&local, sizeof(T));
        writePtr(path, &local, sizeof(T));
    }

    template<typename T>
    SharedArray<T> GTAR::readIndividual(const string &path)
    {
        SharedArray<char> bytes(m_archive.read(path));

        maybeSwapEndian<T>((T*) bytes.get(), bytes.size());
        return *((T*) bytes.get());
    }

    template<typename T>
    T GTAR::readUniform(const string &path)
    {
        SharedArray<char> bytes(m_archive.read(path));

        maybeSwapEndian<T>((T*) bytes.get(), bytes.size());
        return *((T*) bytes.get());
    }

}

#endif
