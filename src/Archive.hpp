// Archive.h
// by Matthew Spellings <mspells@umich.edu>

#include <memory>
#include <vector>
#include <string>
#include <utility>

#include "vogl_miniz_zip.h"
#include "SharedArray.hpp"

#ifndef __ARCHIVE_HPP_
#define __ARCHIVE_HPP_

namespace gtar{

    using std::auto_ptr;
    using std::string;
    using std::pair;
    using std::vector;

    enum OpenMode {Read, Write, Append};

    enum CompressMode {NoCompress, FastCompress, MediumCompress, SlowCompress};

    class Archive
    {
    public:
        Archive(const string &filename, const OpenMode mode);

        ~Archive();

        void writeVec(const string &path, const vector<char> &contents, CompressMode mode);

        void writePtr(const string &path, const void *contents,
                      const size_t byteLength, CompressMode mode);

        SharedArray<char> read(const string &path);

    private:
        const string m_filename;
        const OpenMode m_mode;
        mz_zip_archive m_archive;
    };

}

#endif
