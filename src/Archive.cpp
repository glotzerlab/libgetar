// Archive.cpp
// by Matthew Spellings <mspells@umich.edu>

#include <sstream>
#include <stdexcept>

#include "Archive.hpp"
#include "vogl_miniz_zip.h"
#include "SharedArray.hpp"

namespace gtar{

    using std::auto_ptr;
    using std::runtime_error;
    using std::string;
    using std::stringstream;
    using std::vector;

    Archive::Archive(const string &filename, const OpenMode mode):
        m_filename(filename), m_mode(mode), m_archive()
    {
        mz_zip_zero_struct(&m_archive);

        if(m_mode == Write)
        {
            mz_bool success(
                mz_zip_writer_init_file(&m_archive, filename.c_str(), 0, MZ_ZIP_FLAG_WRITE_ZIP64));

            if(!success)
                throw runtime_error("Error opening file for write");
        }
        else if(m_mode == Read)
        {
            mz_bool success(
                mz_zip_reader_init_file(&m_archive, filename.c_str(),
                                        MZ_ZIP_FLAG_CASE_SENSITIVE, 0, 0));

            if(!success)
                throw runtime_error("Error opening file for read");
        }
        else
        {
            mz_bool success(
                mz_zip_reader_init_file(&m_archive, filename.c_str(),
                                        MZ_ZIP_FLAG_CASE_SENSITIVE, 0, 0));

            if(!success)
                throw runtime_error("Error opening file for append (stage 1)");

            success = mz_zip_writer_init_from_reader(&m_archive, filename.c_str(), MZ_ZIP_FLAG_WRITE_ZIP64);

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

    void Archive::writeVec(const string &path, const vector<char> &contents,
                           CompressMode mode)
    {
        writePtr(path, (void*) &contents[0], contents.size(), mode);
    }

    void Archive::writePtr(const string &path, const void *contents,
                           const size_t byteLength, CompressMode mode)
    {
        if(m_mode == Read)
            throw runtime_error("Can't write to an archive opened for reading");

        mz_uint flags(MZ_ZIP_FLAG_CASE_SENSITIVE);

        switch(mode)
        {
        case FastCompress:
            flags |= MZ_BEST_SPEED;
            break;
        case MediumCompress:
            flags |= MZ_DEFAULT_LEVEL;
            break;
        case SlowCompress:
            flags |= MZ_BEST_COMPRESSION;
            break;
        case NoCompress:
        default:
            flags |= MZ_NO_COMPRESSION;
        }

        bool success(
            mz_zip_writer_add_mem(&m_archive, path.c_str(), contents,
                                  byteLength, flags));

        if(!success)
            throw runtime_error("Failed adding a file to archive");
    }

    SharedArray<char> Archive::read(const string &path)
    {
        if(m_mode != Read)
            throw runtime_error("Can't read from a file not opened for reading");

        mz_uint32 fileIndex(0);
        bool success(mz_zip_locate_file(&m_archive, path.c_str(), NULL, MZ_ZIP_FLAG_CASE_SENSITIVE, &fileIndex));
        mz_zip_archive_file_stat stat;

        if(!success)
            return SharedArray<char>();

        mz_zip_file_stat(&m_archive, fileIndex, &stat);

        auto_ptr<char> result(new char[stat.m_uncomp_size]);
        success = mz_zip_extract_to_mem(&m_archive, fileIndex, result.get(), stat.m_uncomp_size, MZ_ZIP_FLAG_CASE_SENSITIVE);

        if(!success)
            throw runtime_error("Failed extracting file " + path);

        return SharedArray<char>(result.release(), stat.m_uncomp_size);
    }

    unsigned int Archive::size()
    {
        return mz_zip_get_num_files(&m_archive);
    }

    string Archive::getItemName(unsigned int index)
    {
        const size_t bufSize(64);
        stringstream name;
        char buf[bufSize];
        unsigned int len(0);

        do
        {
            len = mz_zip_get_filename(&m_archive, index, buf, bufSize);
            name << string(buf, len);
        }
        while(len < bufSize);

        return name.str();
    }
}
