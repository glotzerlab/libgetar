// Archive.hpp
// by Matthew Spellings <mspells@umich.edu>

#include <memory>
#include <vector>
#include <string>
#include <utility>

#include "SharedArray.hpp"

#ifndef __ARCHIVE_HPP_
#define __ARCHIVE_HPP_

#ifdef GTAR_NAMESPACE_PARENT
namespace GTAR_NAMESPACE_PARENT{
#endif

namespace gtar{

    // Modes in which we can open a file
    enum OpenMode {Read, Write, Append};

    // Varying degrees to which files can be compressed
    enum CompressMode {NoCompress, FastCompress, MediumCompress, SlowCompress};

    // Archive abstraction layer. Pure virtual interface for archive
    // (i.e., a handle to a file) functionality
    class Archive
    {
    public:
        // Destructor: Clean up memory used
        virtual ~Archive() = 0;

        // Close the archive
        virtual void close() = 0;

        // Write a char vector of bytes to the given path within the
        // archive with the given compress mode
        virtual void writeVec(const std::string &path, const std::vector<char> &contents,
                              CompressMode mode, bool immedate=false);

        // Write the contents of a pointer to the given path within
        // the archive with the given compress mode
        virtual void writePtr(const std::string &path, const void *contents,
                              const size_t byteLength, CompressMode mode,
                              bool immediate=false) = 0;

        virtual void beginBulkWrites() = 0;
        virtual void endBulkWrites() = 0;

        // Read the contents of the given location within the archive
        virtual SharedArray<char> read(const std::string &path) = 0;

        // Return the number of files stored in the archive
        virtual unsigned int size() = 0;
        // Return the name of the file with the given numerical index
        virtual std::string getItemName(unsigned int index) = 0;
    };

}

#ifdef GTAR_NAMESPACE_PARENT
}
#endif

#endif
