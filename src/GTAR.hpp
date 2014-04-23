// GTAR.hpp
// by Matthew Spellings <mspells@umich.edu>

#include <string>
#include <vector>

#include "Archive.hpp"
#include "SharedArray.hpp"

#ifndef __GTAR_HPP_
#define __GTAR_HPP_

#define IS_BIG_ENDIAN (*(uint16_t *)"\0\xff" < 0x100)

namespace gtar{

    using std::string;
    using std::vector;

    // Swap bytes to little-endian order if necessary
    template<typename T>
    void toLittleEndian(char *target, size_t byteLength);

    class GTAR
    {
    public:
        GTAR(const string &filename, const OpenMode mode):
            m_archive(filename, mode) {}

        // Most generic functions. Toss some bytes into a location.
        void writeString(const string &path, const string &contents);
        void writeBytes(const string &path, const vector<char> &contents);
        void writePtr(const string &path, const void *contents, const size_t byteLength);

        // Write individual and uniform binary properties to the specified
        // location, converting to little endian if necessary.
        template<typename iter, typename T>
        void writeIndividual(const string &path, const iter &start, const iter &end);
        template<typename T>
        void writeUniform(const string &path, const T &val);

        template<typename T>
        SharedArray<T> readIndividual(const string &path);
        template<typename T>
        T readUniform(const string &path);
        SharedArray<char> readBytes(const string &path);

    private:
        Archive m_archive;
    };

    // Swap the bytes of a series of characters if this is a big-endian machine
    template<typename T>
    void maybeSwapEndian(T *target, size_t byteLength)
    {
        char *recast((char*) target);

        if(byteLength % sizeof(T) != 0)
            throw runtime_error("Trying to convert an incorrect number of bytes to little-endian");

        if(IS_BIG_ENDIAN)
        {
            for(size_t i(0); i < byteLength/sizeof(T); ++i)
            {
                for(size_t j(0); j < sizeof(T)/2; ++j)
                    swap(recast[i*sizeof(T) + j], recast[(i + 1)*sizeof(T) - j - 1]);
            }
        }
    }

    template<typename iter, typename T>
    void GTAR::writeIndividual(const string &path, const iter &start, const iter &end)
    {
        vector<T> buffer(start, end);

        maybeSwapEndian<T>(&buffer[0], buffer.size()*sizeof(T));
        writePtr(path, &buffer[0], buffer.size()*sizeof(T));
    }

    template<typename T>
    void GTAR::writeUniform(const string &path, const T &val)
    {
        T local(val);

        maybeSwapEndian<T>(&local, sizeof(T));
        writePtr(path, &local, sizeof(T));
    }

    template<typename T>
    SharedArray<T> GTAR::readIndividual(const string &path)
    {
        SharedArray<char> bytes(m_archive.read(path));

        maybeSwapEndian<T>((T*) bytes.get(), bytes.size());
        return *((T*) bytes.get());
    }

    template<typename T>
    T GTAR::readUniform(const string &path)
    {
        SharedArray<char> bytes(m_archive.read(path));

        maybeSwapEndian<T>((T*) bytes.get(), bytes.size());
        return *((T*) bytes.get());
    }

}

#endif
