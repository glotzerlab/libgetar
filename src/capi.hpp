// capi.hpp
// by Matthew Spellings <mspells@umich.edu>
// provides a C-api wrapper to libgetar functionality

#include "GTAR.hpp"
#include "Record.hpp"

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

    unsigned int queryRecordCount(GTAR *gtar, const char *group,
                                  const char *name, const char *suffix,
                                  unsigned int behavior, unsigned int format,
                                  unsigned int resolution);

    char *getRecordIndex(GTAR *gtar, const char *group, const char *name,
                        const char *suffix, unsigned int behavior,
                        unsigned int format, unsigned int resolution,
                        unsigned int index, size_t *byteLength);

    char *readRecord(GTAR *gtar, const char *group, const char *name,
                     const char *index, const char *suffix, unsigned int behavior,
                     unsigned int format, unsigned int resolution, size_t *byteLength);

    unsigned int enumOpenMode_Read();
    unsigned int enumOpenMode_Write();
    unsigned int enumOpenMode_Append();

    unsigned int enumCompressMode_NoCompress();
    unsigned int enumCompressMode_FastCompress();
    unsigned int enumCompressMode_MediumCompress();
    unsigned int enumCompressMode_SlowCompress();
}
