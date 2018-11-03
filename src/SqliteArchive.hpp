// SqliteArchive.hpp
// by Matthew Spellings <mspells@umich.edu>

#include <string>
#include "../sqlite3/sqlite3.h"

#include "Archive.hpp"


#ifndef __SQLITE_ARCHIVE_HPP_
#define __SQLITE_ARCHIVE_HPP_

#ifdef GTAR_NAMESPACE_PARENT
namespace GTAR_NAMESPACE_PARENT{
#endif

namespace gtar{

    // Simple interface for sqlite files
    class SqliteArchive: public Archive
    {
    public:
        // Constructor: Open or create an archive object with the
        // given filename and access mode
        SqliteArchive(const std::string &filename, const OpenMode mode);

        // Destructor: Clean up memory used
        virtual ~SqliteArchive();

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
        // Name of the archive file we're accessing
        const std::string m_filename;
        // How we're accessing the archive
        const OpenMode m_mode;
        // Cached list of paths in archive
        std::vector<std::string> m_fileNames;

        // Pointer to our db handle
        sqlite3 *m_connection;

        sqlite3_stmt *m_begin_stmt;
        sqlite3_stmt *m_end_stmt;
        sqlite3_stmt *m_rollback_stmt;
        sqlite3_stmt *m_insert_filename_stmt;
        sqlite3_stmt *m_insert_contents_stmt;
        sqlite3_stmt *m_select_contents_stmt;
        sqlite3_stmt *m_list_files_stmt;
    };
}

#ifdef GTAR_NAMESPACE_PARENT
}
#endif

#endif
