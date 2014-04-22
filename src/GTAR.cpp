// GTAR.cpp
// by Matthew Spellings <mspells@umich.edu>

#include "GTAR.hpp"
#include "SharedArray.hpp"

#include <algorithm>
#include <stdexcept>
#include <stdint.h>

#define IS_BIG_ENDIAN (*(uint16_t *)"\0\xff" < 0x100)

namespace gtar{

    using std::runtime_error;
    using std::string;
    using std::swap;
    using std::vector;

    // Swap the bytes of a series of characters if this is a big-endian machine
    template<typename T>
    void maybeSwapEndian(char *target, size_t byteLength)
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

    template<typename iter, typename T>
    void GTAR::writeIndividual(const string &path, const iter &start, const iter &end)
    {
        vector<T> buffer(start, end);

        maybeSwapEndian<T>((char*) &buffer[0], buffer.size()*sizeof(T));
        writePtr(path, &buffer[0], buffer.size()*sizeof(T));
    }

    template<typename T>
    void GTAR::writeUniform(const string &path, const T &val)
    {
        T local(val);

        maybeSwapEndian<T>((char*) &local, sizeof(T));
        writePtr(path, &local, sizeof(T));
    }

    template<typename T>
    SharedArray<T> GTAR::readIndividual(const string &path)
    {
        SharedArray<char> bytes(m_archive.read(path));

        maybeSwapEndian<T>((char*) bytes.get(), bytes.size());
        return *((T*) bytes.get());
    }

    template<typename T>
    T GTAR::readUniform(const string &path)
    {
        SharedArray<char> bytes(m_archive.read(path));

        maybeSwapEndian<T>((char*) bytes.get(), bytes.size());
        return *((T*) bytes.get());
    }

    SharedArray<char> GTAR::readBytes(const string &path)
    {
        return m_archive.read(path);
    }
}
