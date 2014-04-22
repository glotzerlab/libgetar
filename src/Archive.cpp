// Archive.cpp
// by Matthew Spellings <mspells@umich.edu>

#include <stdexcept>

#include "Archive.hpp"
#include <archive_entry.h>

namespace gtar{

    using std::auto_ptr;
    using std::runtime_error;
    using std::string;
    using std::vector;

    Archive::Archive(const string &filename, const OpenMode mode):
        m_filename(filename), m_mode(mode), m_archiveptr()
    {
        if(m_mode == Write)
        {
            m_archiveptr.reset(archive_write_new());

            archive_write_set_compression_none(m_archiveptr.get());
            archive_write_set_format_zip(m_archiveptr.get());

            int err(archive_write_open_filename(m_archiveptr.get(), m_filename.c_str()));

            if(err != ARCHIVE_OK)
                throw runtime_error("Archive opening for write failed");
        }
        else
        {
            m_archiveptr.reset(archive_read_new());

            // Enable any formats that libarchive supports
            archive_read_support_filter_all(m_archiveptr.get());
            archive_read_support_format_all(m_archiveptr.get());

            int err(archive_read_open_filename(m_archiveptr.get(), m_filename.c_str(), 8192));

            if(err != ARCHIVE_OK)
                throw runtime_error("Archive opening for read failed");
        }
    }

    Archive::~Archive()
    {
        if(m_mode == Write)
            archive_write_free(m_archiveptr.release());
        else
            archive_read_free(m_archiveptr.release());
    }

    void Archive::writeVec(const string &path, const vector<char> &contents)
    {
        writePtr(path, (void*) &contents[0], contents.size());
    }

    void Archive::writePtr(const string &path, const void *contents, const size_t byteLength)
    {
        if(m_mode == Read)
            throw runtime_error("Can't write to an archive opened for reading");

        archive_entry *entry(archive_entry_new());

        if(!entry)
            throw runtime_error("Failed allocating a libarchive archive_entry");

        archive_entry_set_pathname(entry, path.c_str());
        archive_entry_set_size(entry, byteLength);
        archive_entry_set_filetype(entry, AE_IFREG);
        archive_entry_set_perm(entry, 0644);

        archive_write_header(m_archiveptr.get(), entry);
        archive_write_data(m_archiveptr.get(), contents, byteLength);

        archive_entry_free(entry);
    }
}
