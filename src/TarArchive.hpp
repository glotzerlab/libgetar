// TarArchive.hpp
// by Matthew Spellings <mspells@umich.edu>

#include <fstream>
#include <map>
#include <memory>
#include <vector>
#include <string>
#include <utility>

#include "Archive.hpp"

#ifndef __TAR_ARCHIVE_HPP_
#define __TAR_ARCHIVE_HPP_

// tar format reference: http://pic.dhe.ibm.com/infocenter/zos/v1r13/index.jsp?topic=%2Fcom.ibm.zos.r13.bpxa500%2Ftaf.htm

namespace gtar{

    using std::auto_ptr;
    using std::fstream;
    using std::map;
    using std::string;
    using std::pair;
    using std::vector;

    // Simple interface for tar files
    class TarArchive: public Archive
    {
    public:
        // Constructor: Open or create an archive object with the
        // given filename and access mode
        TarArchive(const string &filename, const OpenMode mode);

        // Destructor: Clean up memory used
        virtual ~TarArchive();

        // Close the archive
        virtual void close();

        // Write the contents of a pointer to the given path within
        // the archive with the given compress mode
        virtual void writePtr(const string &path, const void *contents,
                              const size_t byteLength, CompressMode mode,
                              bool immediate=false);

        virtual void beginBulkWrites();
        virtual void endBulkWrites();

        // Read the contents of the given location within the archive
        virtual SharedArray<char> read(const string &path);

        // Return the number of files stored in the archive
        virtual unsigned int size();
        // Return the name of the file with the given numerical index
        virtual string getItemName(unsigned int index);

    private:
        // Name of the archive file we're accessing
        const string m_filename;
        // How we're accessing the archive
        const OpenMode m_mode;
        // Handle to the file we're accessing
        fstream m_file;

        // All the file names present in the file, in file order
        vector<string> m_fileNames;
        // Map from filenames to offsets within the file
        map<string, size_t> m_fileOffsets;
        // Map from filenames to file sizes
        map<string, size_t> m_fileSizes;
    };

    struct TarHeader
    {
        char name[100];
        char mode[8];
        char uid[8];
        char gid[8];
        char size[12];
        char mtime[12];
        char chksum[8];
        char typeflag[1];
        char linkname[100];
        char magic[6];
        char version[2];
        char uname[32];
        char gname[32];
        char devmajor[8];
        char devminor[8];
        char prefix[155];
        char padding[12];
    };

}

#endif
