// DirArchive.hpp
// by Matthew Spellings <mspells@umich.edu>

#include <fstream>
#include <map>
#include <memory>
#include <vector>
#include <set>
#include <string>
#include <utility>

#include "Archive.hpp"

#ifndef __DIR_ARCHIVE_HPP_
#define __DIR_ARCHIVE_HPP_

namespace gtar{

    using std::auto_ptr;
    using std::fstream;
    using std::map;
    using std::set;
    using std::string;
    using std::pair;
    using std::vector;

    // Simple interface for dir files
    class DirArchive: public Archive
    {
    public:
        // Constructor: Open or create an archive object with the
        // given filename and access mode
        DirArchive(const string &filename, const OpenMode mode);

        // Destructor: Clean up memory used
        virtual ~DirArchive();

        // Close the archive
        virtual void close();

        // Write the contents of a pointer to the given path within
        // the archive with the given compress mode
        virtual void writePtr(const string &path, const void *contents,
                              const size_t byteLength, CompressMode mode,
                              bool immediate=false);

        virtual void beginBulkWrites();
        virtual void endBulkWrites();

        // Read the contents of the given location within the archive
        virtual SharedArray<char> read(const string &path);

        // Return the number of files stored in the archive
        virtual unsigned int size();
        // Return the name of the file with the given numerical index
        virtual string getItemName(unsigned int index);

    private:
        // Helper function to recursively search through a directory
        void searchDirectory(const string &path);

        // Name of the archive file we're accessing
        const string m_filename;
        // How we're accessing the archive
        const OpenMode m_mode;

        // All the directories which have been created
        set<string> m_createdDirectories;
        // All the file names we found in the file, in file order
        vector<string> m_fileNames;
    };
}

#endif
