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

    void GTAR::writeString(const string &path, const string &contents)
    {
        m_archive.writePtr(path, contents.data(), contents.size());
    }

    void GTAR::writeBytes(const string &path, const vector<char> &contents)
    {
        m_archive.writeVec(path, contents);
    }

    void GTAR::writePtr(const string &path, const void *contents, const size_t byteLength)
    {
        m_archive.writePtr(path, contents, byteLength);
    }

    SharedArray<char> GTAR::readBytes(const string &path)
    {
        return m_archive.read(path);
    }
}
