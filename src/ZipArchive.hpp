// ZipArchive.hpp
// by Matthew Spellings <mspells@umich.edu>

#include <map>
#include <memory>
#include <vector>
#include <string>
#include <utility>

#include "Archive.hpp"
#include "../miniz/miniz.h"

#ifndef __ZIP_ARCHIVE_HPP_
#define __ZIP_ARCHIVE_HPP_

#ifdef GTAR_NAMESPACE_PARENT
namespace GTAR_NAMESPACE_PARENT{
#endif

namespace gtar{

    // ZipArchive abstraction layer. Thin wrapper over miniz
    // functionality.
    class ZipArchive: public Archive
    {
    public:
        // Constructor: Open or create an archive object with the
        // given filename and access mode
        ZipArchive(const std::string &filename, const OpenMode mode);

        // Destructor: Clean up memory used
        virtual ~ZipArchive();

        // Close the archive
        virtual void close();

        // Write the contents of a pointer to the given path within
        // the archive with the given compress mode
        virtual void writePtr(const std::string &path, const void *contents,
                              const size_t byteLength, CompressMode mode,
                              bool immediate=false);

        virtual void beginBulkWrites();
        virtual void endBulkWrites();

        // Read the contents of the given location within the archive
        virtual SharedArray<char> read(const std::string &path);

        // Return the number of files stored in the archive
        virtual unsigned int size();
        // Return the name of the file with the given numerical index
        virtual std::string getItemName(unsigned int index);

    private:
        // fill m_path_map
        void fillPathMap();

        // Name of the archive file we're accessing
        const std::string m_filename;
        // How we're accessing the archive
        const OpenMode m_mode;
        // Stored miniz archive object
        mz_zip_archive m_archive;
        // Stored map of path -> last archive index that contains the path
        std::map<std::string, size_t> m_path_map;
    };

    // Helper function to be accessed from python. Checks if a zip
    // archive existing at the given name is in zip64 format.
    bool isZip64(const std::string &filename);

}

#ifdef GTAR_NAMESPACE_PARENT
}
#endif

#endif
