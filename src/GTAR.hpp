// GTAR.hpp
// by Matthew Spellings <mspells@umich.edu>

#include <string>
#include <vector>

#include "Archive.hpp"
#include "SharedArray.hpp"

#ifndef __GTAR_HPP_
#define __GTAR_HPP_

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
}

#include "GTAR.cpp"

#endif
