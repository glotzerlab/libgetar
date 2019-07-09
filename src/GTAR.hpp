// GTAR.hpp
// by Matthew Spellings <mspells@umich.edu>

#include <algorithm>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

// if C++11 or greater, use unique_ptr
#if __cplusplus > 199711L
#define gtar_unique_ptr std::unique_ptr
#else
#define gtar_unique_ptr std::auto_ptr
#endif

#include "Archive.hpp"
#include "DirArchive.hpp"
#include "SqliteArchive.hpp"
#include "TarArchive.hpp"
#include "ZipArchive.hpp"
#include "SharedArray.hpp"
#include "Record.hpp"

#ifndef __GTAR_HPP_
#define __GTAR_HPP_

#ifdef GTAR_NAMESPACE_PARENT
namespace GTAR_NAMESPACE_PARENT{
#endif

namespace gtar{

    bool littleEndian();
    // const static bool IS_BIG_ENDIAN = !littleEndian();
    const static bool IS_BIG_ENDIAN = false;

    /// Swap the endianness of all elements, if necessary on this
    /// architecture
    template<typename T>
    void maybeSwapEndian(char *target, size_t byteLength);

    /// Special sorter for index strings such that shorter strings
    /// appear first
    class IndexCompare
    {
    public:
        bool operator()(const std::string &a, const std::string &b) const;
    };

    /// Accessor interface for a trajectory archive
    class GTAR
    {
    public:
        typedef std::set<std::string, IndexCompare> indexSet;

        class BulkWriter
        {
        public:
            /// Create a new BulkWriter on an archive. Only one should
            /// exist for any archive at a time.
            BulkWriter(GTAR &archive);

            /// Clean up the BulkWriter data. Causes all writes to be
            /// performed.
            ~BulkWriter();

            /// Write a string to the given location
            void writeString(const std::string &path, const std::string &contents,
                             CompressMode mode);
            /// Write a bytestring to the given location
            void writeBytes(const std::string &path, const std::vector<char> &contents,
                            CompressMode mode);
            /// Write the contents of a pointer to the given location
            void writePtr(const std::string &path, const void *contents,
                          const size_t byteLength, CompressMode mode);

            /// Write an individual binary property to the specified
            /// location, converting to little endian if necessary.
            template<typename iter, typename T>
            void writeIndividual(const std::string &path, const iter &start,
                                 const iter &end, CompressMode mode);
            /// Write a uniform binary property to the specified location,
            /// converting to little endian if necessary.
            template<typename T>
            void writeUniform(const std::string &path, const T &val);

        private:
            GTAR &m_archive;
        };

        /// Constructor. Opens the file at filename in the given
        /// mode. The format of the file depends on the extension of
        /// filename.
        GTAR(const std::string &filename, const OpenMode mode);

        /// Manually close the opened archive (it automatically closes
        /// itself upon destruction)
        void close();

        /// Write a string to the given location
        void writeString(const std::string &path, const std::string &contents,
                         CompressMode mode);
        /// Write a bytestring to the given location
        void writeBytes(const std::string &path, const std::vector<char> &contents,
                        CompressMode mode);
        /// Write the contents of a pointer to the given location
        void writePtr(const std::string &path, const void *contents,
                      const size_t byteLength, CompressMode mode);

        /// Write an individual binary property to the specified
        /// location, converting to little endian if necessary.
        template<typename iter, typename T>
        void writeIndividual(const std::string &path, const iter &start,
                             const iter &end, CompressMode mode);
        /// Write a uniform binary property to the specified location,
        /// converting to little endian if necessary.
        template<typename T>
        void writeUniform(const std::string &path, const T &val);

        /// Read an individual binary property to the specified
        /// location, converting from little endian if necessary.
        template<typename T>
        SharedArray<T> readIndividual(const std::string &path);
        /// Read a uniform binary property to the specified location,
        /// converting from little endian if necessary.
        template<typename T>
        SharedPtr<T> readUniform(const std::string &path);
        /// Read a bytestring from the specified location
        SharedArray<char> readBytes(const std::string &path);

        /// Query all of the records in the archive. These will all
        /// have empty indices.
        std::vector<Record> getRecordTypes() const;
        /// Query the indices associated with a given record. The
        /// record is not required to have a null index.
        std::vector<std::string> queryFrames(const Record &target) const;

    private:
        /// Write a string to the given location
        void writeString(const std::string &path, const std::string &contents,
                         CompressMode mode, bool immediate);
        /// Write a bytestring to the given location
        void writeBytes(const std::string &path, const std::vector<char> &contents,
                        CompressMode mode, bool immediate);
        /// Write the contents of a pointer to the given location
        void writePtr(const std::string &path, const void *contents,
                      const size_t byteLength, CompressMode mode,
                      bool immediate);

        /// Write an individual binary property to the specified
        /// location, converting to little endian if necessary.
        template<typename iter, typename T>
        void writeIndividual(const std::string &path, const iter &start,
                             const iter &end, CompressMode mode, bool immediate);
        /// Write a uniform binary property to the specified location,
        /// converting to little endian if necessary.
        template<typename T>
        void writeUniform(const std::string &path, const T &val, bool immediate);

        /// Optimize the archive writes for multiple records at once;
        /// must be accompanied by a call to endBulkWrites()
        void beginBulkWrites();
        /// Flush writes out of temporary buffers
        void endBulkWrites();

        /// Insert a record into the set of cached records
        void insertRecord(const std::string &path);

        /// The archive abstraction object we'll use
        gtar_unique_ptr<Archive> m_archive;

        /// Cached record objects
        std::map<Record, indexSet> m_records;
        std::map<Record, std::vector<std::string> > m_indexedRecords;
    };

    /// Swap the bytes of a series of characters if this is a big-endian machine
    template<typename T>
    void maybeSwapEndian(T *target, size_t byteLength)
    {
        char *recast((char*) target);

        if(byteLength % sizeof(T) != 0)
            throw std::runtime_error("Trying to convert an incorrect number of bytes to little-endian");

        if(IS_BIG_ENDIAN)
        {
            for(size_t i(0); i < byteLength/sizeof(T); ++i)
            {
                for(size_t j(0); j < sizeof(T)/2; ++j)
                    std::swap(recast[i*sizeof(T) + j], recast[(i + 1)*sizeof(T) - j - 1]);
            }
        }
    }

    template<typename iter, typename T>
    void GTAR::BulkWriter::writeIndividual(const std::string &path, const iter &start,
                               const iter &end, CompressMode mode)
    {
        m_archive.writeIndividual<iter, T>(path, start, end, mode, false);
    }

    template<typename T>
    void GTAR::BulkWriter::writeUniform(const std::string &path, const T &val)
    {
        m_archive.writeUniform<T>(path, val, false);
    }

    template<typename iter, typename T>
    void GTAR::writeIndividual(const std::string &path, const iter &start,
                               const iter &end, CompressMode mode)
    {
        writeIndividual<iter, T>(path, start, end, mode, true);
    }

    template<typename T>
    void GTAR::writeUniform(const std::string &path, const T &val)
    {
        writeUniform<T>(path, val, true);
    }

    template<typename iter, typename T>
    void GTAR::writeIndividual(const std::string &path, const iter &start,
                               const iter &end, CompressMode mode, bool immediate)
    {
        std::vector<T> buffer(start, end);

        maybeSwapEndian<T>(&buffer[0], buffer.size()*sizeof(T));
        writePtr(path, (void*) &buffer[0], buffer.size()*sizeof(T), mode, immediate);
    }

    template<typename T>
    void GTAR::writeUniform(const std::string &path, const T &val, bool immediate)
    {
        T local(val);

        maybeSwapEndian<T>(&local, sizeof(T));
        writePtr(path, (void*) &local, sizeof(T), NoCompress, immediate);
    }

    template<typename T>
    SharedArray<T> GTAR::readIndividual(const std::string &path)
    {
        SharedArray<char> bytes(m_archive->read(path));
        maybeSwapEndian<T>((T*) bytes.get(), bytes.size());

        const size_t resultSize(bytes.size()/sizeof(T));

        if(resultSize*sizeof(T) != bytes.size())
            throw std::runtime_error("Trying to coerce the wrong number of bytes into an individual property");

        SharedArray<T> result((T*) bytes.disown(), resultSize);

        return result;
    }

    template<typename T>
    SharedPtr<T> GTAR::readUniform(const std::string &path)
    {
        SharedArray<char> bytes(m_archive->read(path));

        maybeSwapEndian<T>((T*) bytes.get(), bytes.size());

        if(bytes.size())
            return SharedPtr<T>((SharedArray<T>&) bytes);
        else
            return SharedPtr<T>();
    }

}

#ifdef GTAR_NAMESPACE_PARENT
}
#endif

#endif
