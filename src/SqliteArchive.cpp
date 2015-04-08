// SqliteArchive.cpp
// by Matthew Spellings <mspells@umich.edu>

#include <cstdio>
#include <cstring>
#include <sstream>
#include <stdexcept>

#ifdef ENABLE_SQLITE

#include "lz4/lz4.h"
#include "lz4/lz4hc.h"
#include "SqliteArchive.hpp"

namespace gtar{

    using std::runtime_error;
    using std::string;
    using std::stringstream;

    SqliteArchive::SqliteArchive(const string &filename, const OpenMode mode):
        m_filename(filename), m_mode(mode), m_fileNames(), m_connection(0),
        m_begin_stmt(0), m_end_stmt(0), m_rollback_stmt(0),
        m_insert_filename_stmt(0), m_insert_contents_stmt(0),
        m_select_contents_stmt(0), m_list_files_stmt(0)
    {
        sqlite3_initialize();

        const int flags(mode == Read? SQLITE_OPEN_READONLY:
                        SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE);

        if(mode == Write)
            remove(filename.c_str());

        const int openStatus(sqlite3_open_v2(filename.c_str(), &m_connection, flags, 0));

        if(openStatus)
        {
            stringstream result;

            result << "Can't open sqlite database " << filename << ": ";
            result << sqlite3_errmsg(m_connection);

            throw runtime_error(result.str());
        }

        char *errmsg(0);
        int execStatus(sqlite3_exec(m_connection,
                                    "CREATE TABLE IF NOT EXISTS file_list ("
                                    "path TEXT PRIMARY KEY ON CONFLICT REPLACE NOT NULL,"
                                    "uncompressed_size INTEGER NOT NULL,"
                                    "compressed_size INTEGER NOT NULL,"
                                    "compress_level INTEGER NOT NULL"
                                    ");", 0, 0, &errmsg));
        if(execStatus != SQLITE_OK)
        {
            stringstream result;
            result << "Couldn't create file_list table in sqlite database: ";
            result << errmsg;
            sqlite3_free(errmsg);
            throw runtime_error(result.str());
        }

        execStatus = sqlite3_exec(m_connection,
                                  "CREATE TABLE IF NOT EXISTS file_contents ("
                                  "path TEXT REFERENCES file_list (path) ON "
                                  "DELETE CASCADE ON UPDATE CASCADE,"
                                  "contents BLOB);",
                                  0, 0, &errmsg);
        if(execStatus != SQLITE_OK)
        {
            stringstream result;
            result << "Couldn't create file_contents table in sqlite database: ";
            result << errmsg;
            sqlite3_free(errmsg);
            throw runtime_error(result.str());
        }

        execStatus = sqlite3_prepare_v2(m_connection,
                                        "BEGIN TRANSACTION;",
                                        -1, &m_begin_stmt, 0);
        if(execStatus != SQLITE_OK)
        {
            stringstream result;
            result << "Couldn't compile begin statement: ";
            result << errmsg;
            sqlite3_free(errmsg);
            throw runtime_error(result.str());
        }

        execStatus = sqlite3_prepare_v2(m_connection,
                                        "COMMIT;",
                                        -1, &m_end_stmt, 0);
        if(execStatus != SQLITE_OK)
        {
            stringstream result;
            result << "Couldn't compile end statement: ";
            result << errmsg;
            sqlite3_free(errmsg);
            throw runtime_error(result.str());
        }

        execStatus = sqlite3_prepare_v2(m_connection,
                                        "ROLLBACK;",
                                        -1, &m_rollback_stmt, 0);
        if(execStatus != SQLITE_OK)
        {
            stringstream result;
            result << "Couldn't compile rollback statement: ";
            result << errmsg;
            sqlite3_free(errmsg);
            throw runtime_error(result.str());
        }

        execStatus = sqlite3_prepare_v2(m_connection,
                                        "INSERT INTO file_list VALUES "
                                        "(?, ?, ?, ?);",
                                        -1, &m_insert_filename_stmt, 0);
        if(execStatus != SQLITE_OK)
        {
            stringstream result;
            result << "Couldn't compile file_list insert statement: ";
            result << errmsg;
            sqlite3_free(errmsg);
            throw runtime_error(result.str());
        }

        execStatus = sqlite3_prepare_v2(m_connection,
                                        "INSERT INTO file_contents VALUES "
                                        "(?, ?);",
                                        -1, &m_insert_contents_stmt, 0);
        if(execStatus != SQLITE_OK)
        {
            stringstream result;
            result << "Couldn't compile file_contents insert statement: ";
            result << errmsg;
            sqlite3_free(errmsg);
            throw runtime_error(result.str());
        }

        execStatus = sqlite3_prepare_v2(m_connection,
                                        "SELECT file_list.*, file_contents.contents "
                                        "FROM file_list INNER JOIN file_contents "
                                        "ON file_list.path = file_contents.path "
                                        "WHERE file_list.path = ?;",
                                        -1, &m_select_contents_stmt, 0);
        if(execStatus != SQLITE_OK)
        {
            stringstream result;
            result << "Couldn't compile select_contents statement: ";
            result << errmsg;
            sqlite3_free(errmsg);
            throw runtime_error(result.str());
        }

        execStatus = sqlite3_prepare_v2(m_connection,
                                        "SELECT path FROM file_list;",
                                        -1, &m_list_files_stmt, 0);
        if(execStatus != SQLITE_OK)
        {
            stringstream result;
            result << "Couldn't compile list_files statement: ";
            result << errmsg;
            sqlite3_free(errmsg);
            throw runtime_error(result.str());
        }

        if(mode == Read)
        {
            while(sqlite3_step(m_list_files_stmt) == SQLITE_ROW)
            {
                const size_t bytes(sqlite3_column_bytes(m_list_files_stmt, 0));
                const string str((const char*) sqlite3_column_text(m_list_files_stmt, 0), bytes);
                m_fileNames.push_back(str);
            }

            sqlite3_reset(m_list_files_stmt);
        }
    }

    SqliteArchive::~SqliteArchive()
    {
        close();
    }

    void SqliteArchive::close()
    {
        sqlite3_finalize(m_begin_stmt);
        m_begin_stmt = 0;
        sqlite3_finalize(m_end_stmt);
        m_end_stmt = 0;
        sqlite3_finalize(m_rollback_stmt);
        m_rollback_stmt = 0;
        sqlite3_finalize(m_insert_filename_stmt);
        m_insert_filename_stmt = 0;
        sqlite3_finalize(m_insert_contents_stmt);
        m_insert_contents_stmt = 0;
        sqlite3_finalize(m_select_contents_stmt);
        m_select_contents_stmt = 0;
        sqlite3_finalize(m_list_files_stmt);
        m_list_files_stmt = 0;

        sqlite3_close(m_connection);
        m_connection = 0;
    }

    void SqliteArchive::writePtr(const string &path, const void *contents,
                              const size_t byteLength, CompressMode mode)
    {
        if(m_mode == Read)
            throw runtime_error("Can't write to an archive opened for reading");

        char *rawTarget((char*) contents);
        size_t rawSize(byteLength);
        unsigned int rawCompression(0);
        SharedArray<char> compressedBytes;

        if(mode == FastCompress || mode == MediumCompress || mode == SlowCompress)
        {
            // TODO make this safe for very large inputs
            // (LZ4_MAX_INPUT_SIZE = 2 113 929 216 bytes)
            const int maxSize(LZ4_compressBound(byteLength));

            compressedBytes = SharedArray<char>(new char[maxSize], maxSize);
            rawTarget = compressedBytes.get();
            rawSize = LZ4_compress((const char*) contents,
                                   compressedBytes.get(), byteLength);
            rawCompression = 1;
        }

        sqlite3_bind_text(m_insert_filename_stmt, 1, path.c_str(), path.size(), 0);
        sqlite3_bind_int64(m_insert_filename_stmt, 2, byteLength);
        sqlite3_bind_int64(m_insert_filename_stmt, 3, rawSize);
        sqlite3_bind_int(m_insert_filename_stmt, 4, rawCompression);

        sqlite3_bind_text(m_insert_contents_stmt, 1, path.c_str(), path.size(), 0);
        sqlite3_bind_blob64(m_insert_contents_stmt, 2, (const void*) rawTarget,
                            rawSize, 0);

        int status(SQLITE_BUSY);

        while(status == SQLITE_BUSY)
        {
            status = sqlite3_step(m_begin_stmt);
            status = sqlite3_step(m_insert_filename_stmt);
            status = sqlite3_step(m_insert_contents_stmt);
            status = sqlite3_step(m_end_stmt);
        }

        sqlite3_clear_bindings(m_insert_filename_stmt);
        sqlite3_clear_bindings(m_insert_contents_stmt);
        sqlite3_reset(m_begin_stmt);
        sqlite3_reset(m_insert_filename_stmt);
        sqlite3_reset(m_insert_contents_stmt);
        sqlite3_reset(m_end_stmt);

        if(status != SQLITE_DONE)
        {
            stringstream result;
            result << "Error inserting record at " << path << ": ";
            result << sqlite3_errmsg(m_connection);
            throw runtime_error(result.str());
        }
    }

    SharedArray<char> SqliteArchive::read(const std::string &path)
    {
        if(m_mode != Read)
            throw runtime_error("Can't read from a file not opened for reading");

        SharedArray<char> result;

        sqlite3_bind_text(m_select_contents_stmt, 1, path.c_str(), path.size(), 0);

        const int selectResult(sqlite3_step(m_select_contents_stmt));

        if(selectResult == SQLITE_ROW)
        {
            const size_t uncompSize(sqlite3_column_int64(m_select_contents_stmt, 1));
            const size_t compSize(sqlite3_column_int64(m_select_contents_stmt, 2));
            const size_t compLevel(sqlite3_column_int64(m_select_contents_stmt, 3));

            SharedArray<char> rawBytes(new char[compSize], compSize);
            memcpy(rawBytes.get(), sqlite3_column_blob(m_select_contents_stmt, 4), compSize);

            switch(compLevel)
            {
            case 0:
                result = rawBytes;
                break;
            case 1: // LZ4
            {
                SharedArray<char> uncompressed(new char[uncompSize], uncompSize);
                LZ4_decompress_safe((const char*) rawBytes.get(),
                                    uncompressed.get(), compSize, uncompSize);
                result = uncompressed;
            }
                break;
            default:
                stringstream message;
                message << "Error decompressing record at " << path
                        << ": unknown compression " << compLevel;
                throw runtime_error(message.str());
            }
        }
        else if(selectResult != SQLITE_DONE)
        {
            stringstream message;
            message << "Error finding record at " << path << ": ";
            message << sqlite3_errmsg(m_connection);
            sqlite3_reset(m_select_contents_stmt);
            throw runtime_error(message.str());
        }

        sqlite3_reset(m_select_contents_stmt);

        return result;
    }

    unsigned int SqliteArchive::size()
    {
        return m_fileNames.size();
    }

    string SqliteArchive::getItemName(unsigned int index)
    {
        return m_fileNames[index];
    }

}

#endif
