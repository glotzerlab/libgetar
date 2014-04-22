// Archive.cpp
// by Matthew Spellings <mspells@umich.edu>

#include <stdexcept>

#include "Archive.hpp"
#include "miniz.h"

namespace gtar{

    using std::auto_ptr;
    using std::runtime_error;
    using std::string;
    using std::vector;

    Archive::Archive(const string &filename, const OpenMode mode):
        m_filename(filename), m_mode(mode), m_archive()
    {
        if(m_mode == Write)
        {
            mz_bool success(
                mz_zip_writer_init_file(&m_archive, filename.c_str(), 0));

            if(!success)
                throw runtime_error("Error opening file for write");
        }
        else if(m_mode == Read)
        {
            mz_bool success(
                mz_zip_reader_init_file(&m_archive, filename.c_str(),
                                        MZ_ZIP_FLAG_CASE_SENSITIVE));

            if(!success)
                throw runtime_error("Error opening file for read");
        }
        else
        {
            mz_bool success(
                mz_zip_reader_init_file(&m_archive, filename.c_str(),
                                        MZ_ZIP_FLAG_CASE_SENSITIVE));

            if(!success)
                throw runtime_error("Error opening file for append (stage 1)");

            success = mz_zip_writer_init_from_reader(&m_archive, filename.c_str());

            if(!success)
                throw runtime_error("Error opening file for append (stage 2)");
        }
    }

    Archive::~Archive()
    {
        if(m_mode == Write || m_mode == Append)
        {
            mz_zip_writer_finalize_archive(&m_archive);
            mz_zip_writer_end(&m_archive);
        }
        else
            mz_zip_reader_end(&m_archive);
    }

    void Archive::writeVec(const string &path, const vector<char> &contents)
    {
        writePtr(path, (void*) &contents[0], contents.size());
    }

    void Archive::writePtr(const string &path, const void *contents, const size_t byteLength)
    {
        if(m_mode == Read)
            throw runtime_error("Can't write to an archive opened for reading");

        bool success(
            mz_zip_writer_add_mem(&m_archive, path.c_str(), contents,
                                  byteLength, MZ_BEST_SPEED | MZ_ZIP_FLAG_CASE_SENSITIVE));

        if(!success)
            throw runtime_error("Failed adding a file to archive");
    }
}
