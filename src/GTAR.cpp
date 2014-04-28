// GTAR.cpp
// by Matthew Spellings <mspells@umich.edu>

#include "GTAR.hpp"
#include "SharedArray.hpp"

#include <algorithm>
#include <stdexcept>
#include <stdint.h>

namespace gtar{

    using std::runtime_error;
    using std::string;
    using std::swap;
    using std::vector;

    void GTAR::writeString(const string &path, const string &contents, CompressMode mode)
    {
        m_archive.writePtr(path, contents.data(), contents.size(), mode);
    }

    void GTAR::writeBytes(const string &path, const vector<char> &contents, CompressMode mode)
    {
        m_archive.writeVec(path, contents, mode);
    }

    void GTAR::writePtr(const string &path, const void *contents,
                        const size_t byteLength, CompressMode mode)
    {
        m_archive.writePtr(path, contents, byteLength, mode);
    }

    SharedArray<char> GTAR::readBytes(const string &path)
    {
        return m_archive.read(path);
    }
}
