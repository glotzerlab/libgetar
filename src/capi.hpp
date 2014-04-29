// capi.hpp
// by Matthew Spellings <mspells@umich.edu>
// provides a C-api wrapper to libgetar functionality

#include "GTAR.hpp"

extern "C"
{
    using namespace gtar;
    using std::string;

    GTAR *newGTAR(char *filename, const unsigned int mode);
    void delGTAR(GTAR *gtar);

    void writePtr(GTAR *gtar, const char *path, const void *contents,
                  const size_t byteLength, unsigned int mode);

    char *readBytes(GTAR *gtar, const char *path, size_t *byteLength);
    void freeBytes(char *target);

    unsigned int enumOpenMode_Read();
    unsigned int enumOpenMode_Write();
    unsigned int enumOpenMode_Append();

    unsigned int enumCompressMode_NoCompress();
    unsigned int enumCompressMode_FastCompress();
    unsigned int enumCompressMode_MediumCompress();
    unsigned int enumCompressMode_SlowCompress();
}
