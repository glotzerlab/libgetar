// Archive.h
// by Matthew Spellings <mspells@umich.edu>

#include <memory>
#include <vector>
#include <string>

#include "miniz.h"

#ifndef __ARCHIVE_HPP_
#define __ARCHIVE_HPP_

namespace gtar{

    using std::auto_ptr;
    using std::string;
    using std::vector;

    enum OpenMode {Read, Write, Append};

    class Archive
    {
    public:
        Archive(const string &filename, const OpenMode mode);

        ~Archive();

        void writeVec(const string &path, const vector<char> &contents);

        void writePtr(const string &path, const void *contents, const size_t byteLength);
    private:
        const string m_filename;
        const OpenMode m_mode;
        mz_zip_archive m_archive;
    };

}

#endif
