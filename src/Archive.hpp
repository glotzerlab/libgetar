// Archive.h
// by Matthew Spellings <mspells@umich.edu>

#include <memory>
#include <vector>
#include <string>
#include <utility>

#include "vogl_miniz_zip.h"
#include "SharedArray.hpp"

#ifndef __ARCHIVE_HPP_
#define __ARCHIVE_HPP_

namespace gtar{

    using std::auto_ptr;
    using std::string;
    using std::pair;
    using std::vector;

    // Modes in which we can open a file
    enum OpenMode {Read, Write, Append};

    // Varying degrees to which files can be compressed
    enum CompressMode {NoCompress, FastCompress, MediumCompress, SlowCompress};

    // Archive abstraction layer. Thin wrapper over miniz
    // functionality.
    class Archive
    {
    public:
        // Constructor: Open or create an archive object with the
        // given filename and access mode
        Archive(const string &filename, const OpenMode mode);

        // Destructor: Clean up memory used
        ~Archive();

        // Close the archive
        void close();

        // Write a char vector of bytes to the given path within the
        // archive with the given compress mode
        void writeVec(const string &path, const vector<char> &contents, CompressMode mode);

        // Write the contents of a pointer to the given path within
        // the archive with the given compress mode
        void writePtr(const string &path, const void *contents,
                      const size_t byteLength, CompressMode mode);

        // Read the contents of the given location within the archive
        SharedArray<char> read(const string &path);

        // Return the number of files stored in the archive
        unsigned int size();
        // Return the name of the file with the given numerical index
        string getItemName(unsigned int index);

    private:
        // Name of the archive file we're accessing
        const string m_filename;
        // How we're accessing the archive
        const OpenMode m_mode;
        // Stored miniz archive object
        mz_zip_archive m_archive;
    };

}

#endif
