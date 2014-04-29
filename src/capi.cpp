// capi.hpp
// by Matthew Spellings <mspells@umich.edu>
// provides a C-api wrapper to libgetar functionality

#include "GTAR.hpp"
#include "capi.hpp"

extern "C"
{
    using namespace gtar;
    using std::string;

    GTAR *newGTAR(char *filename, const unsigned int mode)
    {
        return new GTAR(string(filename), static_cast<OpenMode>(mode));
    }

    void delGTAR(GTAR *gtar)
    {
        delete gtar;
    }

    void writePtr(GTAR *gtar, const char *path, const void *contents,
                  const size_t byteLength, unsigned int mode)
    {
        gtar->writePtr(string(path), contents, byteLength, static_cast<CompressMode>(mode));
    }

    char *readBytes(GTAR *gtar, const char *path, size_t &byteLength)
    {
        SharedArray<char> arr(gtar->readBytes(string(path)));

        byteLength = arr.size();
        char *result(arr.disown());

        return result;
    }

    unsigned int enumOpenMode_Read(){return static_cast<unsigned int>(Read);}
    unsigned int enumOpenMode_Write(){return static_cast<unsigned int>(Write);}
    unsigned int enumOpenMode_Append(){return static_cast<unsigned int>(Append);}

    unsigned int enumCompressMode_NoCompress(){return static_cast<unsigned int>(NoCompress);}
    unsigned int enumCompressMode_FastCompress(){return static_cast<unsigned int>(FastCompress);}
    unsigned int enumCompressMode_MediumCompress(){return static_cast<unsigned int>(MediumCompress);}
    unsigned int enumCompressMode_SlowCompress(){return static_cast<unsigned int>(SlowCompress);}
}
