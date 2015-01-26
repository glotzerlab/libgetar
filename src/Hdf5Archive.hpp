// Hdf5Archive.hpp
// by Matthew Spellings <mspells@umich.edu>

#include <memory>
#include <vector>
#include <string>
#include <utility>

#ifdef ENABLE_HDF5

#include "Archive.hpp"
#include <H5Cpp.h>

#ifndef __HDF5_ARCHIVE_HPP_
#define __HDF5_ARCHIVE_HPP_

namespace gtar{

    using std::string;
    using std::vector;
    using H5::H5File;

    // Hdf5Archive abstraction layer. Thin wrapper over miniz
    // functionality.
    class Hdf5Archive: public Archive
    {
    public:
        // Constructor: Open or create an archive object with the
        // given filename and access mode
        Hdf5Archive(const string &filename, const OpenMode mode);

        // Destructor: Clean up memory used
        virtual ~Hdf5Archive();

        // Close the archive
        virtual void close();

        // Write the contents of a pointer to the given path within
        // the archive with the given compress mode
        virtual void writePtr(const string &path, const void *contents,
                              const size_t byteLength, CompressMode mode);

        // Read the contents of the given location within the archive
        virtual SharedArray<char> read(const string &path);

        // Return the number of files stored in the archive
        virtual unsigned int size();
        // Return the name of the file with the given numerical index
        virtual string getItemName(unsigned int index);

    private:
        // Name of the archive file we're accessing
        const string m_filename;
        // How we're accessing the archive
        const OpenMode m_mode;
        // Stored miniz archive object
        H5File m_archive;
    };

}

#endif

#endif
