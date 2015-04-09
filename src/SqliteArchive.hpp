// SqliteArchive.hpp
// by Matthew Spellings <mspells@umich.edu>

#ifdef ENABLE_SQLITE

#include <string>
#include <sqlite3.h>

#include "Archive.hpp"


#ifndef __SQLITE_ARCHIVE_HPP_
#define __SQLITE_ARCHIVE_HPP_

namespace gtar{

    using std::string;

    // Simple interface for sqlite files
    class SqliteArchive: public Archive
    {
    public:
        // Constructor: Open or create an archive object with the
        // given filename and access mode
        SqliteArchive(const string &filename, const OpenMode mode);

        // Destructor: Clean up memory used
        virtual ~SqliteArchive();

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
        // Cached list of paths in archive
        vector<string> m_fileNames;

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

#endif

#endif