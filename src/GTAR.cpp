// GTAR.cpp
// by Matthew Spellings <mspells@umich.edu>

#include "GTAR.hpp"

#include <algorithm>
#include <stdint.h>

#define IS_BIG_ENDIAN (*(uint16_t *)"\0\xff" < 0x100)

namespace gtar{

    using std::string;
    using std::swap;
    using std::vector;

    // Swap the bytes of a series of characters if this is a big-endian machine
    template<typename T>
    void toLittleEndian(char *target, size_t byteLength)
    {
        if(byteLength % sizeof(T) != 0)
            throw runtime_error("Trying to convert an incorrect number of bytes to little-endian");

        if(IS_BIG_ENDIAN)
        {
            for(size_t i(0); i < byteLength/sizeof(T); ++i)
            {
                for(size_t j(0); j < sizeof(T)/2; ++j)
                    swap(target[i*sizeof(T) + j], target[(i + 1)*sizeof(T) - j - 1]);
            }
        }
    }

    void GTAR::writeString(const string &path, const string &contents)
    {
        m_archive.writePtr(path, contents.begin(), contents.size());
    }

    void GTAR::writeBytes(const string &path, const vector<char> &contents)
    {
        m_archive.writeVec(path, contents);
    }

    void GTAR::writePtr(const string &path, const void *contents, const size_t byteLength)
    {
        m_archive.writePtr(path, contents, byteLength);
    }

    template<typename iter, typename T>
    void GTAR::writeIndividual(const string &path, const iter &start, const iter &end)
    {
        vector<T> buffer(start, end);

        toLittleEndian(&buffer[0], buffer.size()*sizeof(T));
        writePtr(path, &buffer[0], buffer.size()*sizeof(T));
    }

    template<typename T>
    void GTAR::writeUniform(const string &path, const T &val)
    {
        T local(val);

        toLittleEndian(&local, sizeof(T));
        writePtr(path, &local, sizeof(T));
    }

    template<typename T>
    vector<T> readIndividual(const string &path)
}
