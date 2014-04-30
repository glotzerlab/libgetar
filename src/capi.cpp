// capi.hpp
// by Matthew Spellings <mspells@umich.edu>
// provides a C-api wrapper to libgetar functionality

#include <cstring>
#include <string>

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

    char *readBytes(GTAR *gtar, const char *path, size_t *byteLength)
    {
        SharedArray<char> arr(gtar->readBytes(string(path)));

        *byteLength = arr.size();
        char *result(arr.disown());

        return result;
    }

    void freeBytes(char *target)
    {
        delete[] target;
    }

    unsigned int queryRecordCount(GTAR *gtar, const char *group,
                                  const char *name, const char *suffix,
                                  unsigned int behavior, unsigned int format,
                                  unsigned int resolution)
    {
        Record rec(string(group), string(name), string(), string(suffix),
                   static_cast<Behavior>(behavior), static_cast<Format>(format),
                   static_cast<Resolution>(resolution));

        return gtar->queryRecordCount(rec);
    }

    char *getRecordIndex(GTAR *gtar, const char *group, const char *name,
                        const char *suffix, unsigned int behavior,
                        unsigned int format, unsigned int resolution,
                        unsigned int index, size_t *byteLength)
    {
        Record rec(string(group), string(name), string(), string(suffix),
                   static_cast<Behavior>(behavior), static_cast<Format>(format),
                   static_cast<Resolution>(resolution));

        std::string strresult(gtar->getRecordIndex(rec, index));
        char *result(new char[strresult.length() + 1]);
        std::strcpy(result, strresult.c_str());
        return result;
    }

    char *readRecord(GTAR *gtar, const char *group, const char *name,
                     const char *index, const char *suffix, unsigned int behavior,
                     unsigned int format, unsigned int resolution, size_t *byteLength)
    {
        Record rec(string(group), string(name), string(index), string(suffix),
                   static_cast<Behavior>(behavior), static_cast<Format>(format),
                   static_cast<Resolution>(resolution));

        return readBytes(gtar, rec.getPath().c_str(), byteLength);
    }

    unsigned int enumOpenMode_Read(){return static_cast<unsigned int>(Read);}
    unsigned int enumOpenMode_Write(){return static_cast<unsigned int>(Write);}
    unsigned int enumOpenMode_Append(){return static_cast<unsigned int>(Append);}

    unsigned int enumCompressMode_NoCompress(){return static_cast<unsigned int>(NoCompress);}
    unsigned int enumCompressMode_FastCompress(){return static_cast<unsigned int>(FastCompress);}
    unsigned int enumCompressMode_MediumCompress(){return static_cast<unsigned int>(MediumCompress);}
    unsigned int enumCompressMode_SlowCompress(){return static_cast<unsigned int>(SlowCompress);}
}
