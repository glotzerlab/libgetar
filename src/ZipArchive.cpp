// ZipArchive.cpp
// by Matthew Spellings <mspells@umich.edu>

#include <sstream>
#include <stdexcept>

#include "ZipArchive.hpp"
#include "../miniz/miniz.h"
#include "SharedArray.hpp"

#ifdef GTAR_NAMESPACE_PARENT
namespace GTAR_NAMESPACE_PARENT{
#endif

namespace gtar{

    using std::runtime_error;
    using std::string;
    using std::stringstream;
    using std::vector;

    ZipArchive::ZipArchive(const string &filename, const OpenMode mode):
        m_filename(filename), m_mode(mode), m_archive(), m_path_map()
    {
        mz_zip_zero_struct(&m_archive);

        if(m_mode == Write)
        {
            mz_bool success(
                mz_zip_writer_init_file_v2(&m_archive, filename.c_str(), 0,
                                           MZ_ZIP_FLAG_WRITE_ZIP64 | MZ_ZIP_FLAG_WRITE_ALLOW_READING));

            if(!success)
            {
                stringstream result;
                result << "Error opening file for write: ";
                result << mz_zip_get_error_string(mz_zip_get_last_error(&m_archive));
                throw runtime_error(result.str());
            }
        }
        else if(m_mode == Read)
        {
            mz_bool success(
                mz_zip_reader_init_file_v2(&m_archive, filename.c_str(),
                                           MZ_ZIP_FLAG_CASE_SENSITIVE, 0, 0));

            if(!success)
            {
                stringstream result;
                result << "Error opening file for read: ";
                result << mz_zip_get_error_string(mz_zip_get_last_error(&m_archive));
                throw runtime_error(result.str());
            }
        }
        else //(m_mode == Append)
        {
            mz_bool success(
                mz_zip_reader_init_file_v2(&m_archive, filename.c_str(),
                                           MZ_ZIP_FLAG_CASE_SENSITIVE, 0, 0));

            if(!success)
            {
                stringstream result;
                result << "Error opening file for append (stage 1): ";
                result << mz_zip_get_error_string(mz_zip_get_last_error(&m_archive));
                throw runtime_error(result.str());
            }

            if(!mz_zip_is_zip64(&m_archive))
            {
                stringstream result;
                result << "File " << filename << " is a standard zip archive, "
                       << "but we will only append to zip64-format archives. "
                       << "You can run the gtar.copy python module to create a "
                       << "zip64 format archive.";
                throw runtime_error(result.str());
            }

            success = mz_zip_writer_init_from_reader_v2(&m_archive, filename.c_str(),
                                                        MZ_ZIP_FLAG_WRITE_ZIP64 | MZ_ZIP_FLAG_WRITE_ALLOW_READING);

            if(!success)
            {
                stringstream result;
                result << "Error opening file for append (stage 2): ";
                result << mz_zip_get_error_string(mz_zip_get_last_error(&m_archive));
                throw runtime_error(result.str());
            }
        }

        fillPathMap();
    }

    ZipArchive::~ZipArchive()
    {
        close();
    }

    void ZipArchive::close()
    {
        if(m_mode == Write || m_mode == Append)
        {
            mz_zip_writer_finalize_archive(&m_archive);
            mz_zip_writer_end(&m_archive);
        }
        else
            mz_zip_reader_end(&m_archive);
        mz_zip_zero_struct(&m_archive);
    }

    void ZipArchive::writePtr(const string &path, const void *contents,
                              const size_t byteLength, CompressMode mode,
                              bool immediate)
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
        {
            stringstream result;
            result << "Error adding a file to archive: ";
            result << mz_zip_get_error_string(mz_zip_get_last_error(&m_archive));
            throw runtime_error(result.str());
        }

        m_path_map[path] = size() - 1;
    }

    void ZipArchive::beginBulkWrites()
    {
    }

    void ZipArchive::endBulkWrites()
    {
    }

    void ZipArchive::fillPathMap()
    {
        for(size_t i(0); i < size(); ++i)
            m_path_map[getItemName(i)] = i;
    }

    SharedArray<char> ZipArchive::read(const string &path)
    {
        std::map<std::string, size_t>::iterator iter(m_path_map.find(path));
        size_t fileIndex(0);

        if(iter == m_path_map.end())
            return SharedArray<char>();
        else
            fileIndex = iter->second;

        bool success;
        mz_zip_archive_file_stat stat;
        mz_zip_reader_file_stat(&m_archive, fileIndex, &stat);

        SharedArray<char> result(new char[stat.m_uncomp_size], stat.m_uncomp_size);
        success = mz_zip_reader_extract_to_mem(&m_archive, fileIndex, result.get(), stat.m_uncomp_size, MZ_ZIP_FLAG_CASE_SENSITIVE);

        if(!success)
        {
            stringstream result;
            result << "Failed extracting file " + path + ": ";
            result << mz_zip_get_error_string(mz_zip_get_last_error(&m_archive));
            throw runtime_error(result.str());
        }

        return result;
    }

    unsigned int ZipArchive::size()
    {
        return mz_zip_reader_get_num_files(&m_archive);
    }

    string ZipArchive::getItemName(unsigned int index)
    {
        const unsigned int len(mz_zip_reader_get_filename(&m_archive, index, NULL, 0));
        SharedArray<char> result(new char[len], len);
        mz_zip_reader_get_filename(&m_archive, index, result.get(), len);
        if(len)
            return string(result.get(), len - 1);
        else
            return string();
    }

    bool isZip64(const string &filename)
    {
        mz_zip_archive archive;
        mz_zip_zero_struct(&archive);

        bool success(mz_zip_reader_init_file_v2(&archive, filename.c_str(),
                                                MZ_ZIP_FLAG_CASE_SENSITIVE, 0, 0));

        if(!success)
        {
            stringstream result;
            result << "Error opening file to check zip64 formatting: ";
            result << mz_zip_get_error_string(mz_zip_get_last_error(&archive));
            throw runtime_error(result.str());
        }

        bool result(mz_zip_is_zip64(&archive));

        mz_zip_reader_end(&archive);
        return result;
    }
}

#ifdef GTAR_NAMESPACE_PARENT
}
#endif
