// TarArchive.cpp
// by Matthew Spellings <mspells@umich.edu>

#include <cstring>
#include <sstream>
#include <ctime>
#include <stdexcept>

#include "TarArchive.hpp"

namespace gtar{

    using std::ios_base;
    using std::oct;
    using std::right;
    using std::runtime_error;
    using std::string;
    using std::stringstream;
    using std::vector;

    TarArchive::TarArchive(const string &filename, const OpenMode mode):
        m_filename(filename), m_mode(mode), m_file()
    {
        ios_base::openmode fileMode(ios_base::binary);

        switch(mode)
        {
        case Write:
            fileMode |= (ios_base::out | ios_base::trunc);
            break;
        case Append:
            fileMode |= (ios_base::out | ios_base::app);
            break;
        default:
            fileMode |= ios_base::in;
        }

        m_file.open(filename.c_str(), fileMode);

        if(!m_file.is_open())
        {
            stringstream result;

            result << "Error opening file " << filename << " for ";

            switch(mode)
            {
            case Write:
                result << "write";
                break;
            case Append:
                result << "append";
                break;
            default:
                result << "read";
            }

            throw runtime_error(result.str());
        }
    }

    TarArchive::~TarArchive()
    {
        close();
    }

    void TarArchive::close()
    {
        if(m_file.is_open())
        {
            // pad the end of file with two 512B blocks
            for(size_t i(0); i < 1024; ++i)
                m_file.put('\0');
            m_file.close();
        }
    }

    void TarArchive::writePtr(const string &path, const void *contents,
                              const size_t byteLength, CompressMode mode)
    {
        if(m_mode == Read)
            throw runtime_error("Can't write to an archive opened for reading");

        string prefix, name(path);

        TarHeader recordHeader;
        memset(&recordHeader, 0, sizeof(TarHeader));

        if(path.length() > 99)
        {
            prefix = path.substr(0, path.length() - 100);
            name = path.substr(path.length() - 100, 99);

            if(prefix.length() > 154)
            {
                stringstream message;
                message << "Can't store file with name length " <<
                    path.length() << "!";
                throw runtime_error(message.str());
            }
        }

        name.copy(recordHeader.name, 99);
        prefix.copy(recordHeader.prefix, 154);
        string("ustar").copy(recordHeader.magic, 6);
        // string("0000000").copy(recordHeader.uid, 8);
        // string("0000000").copy(recordHeader.gid, 8);

        stringstream fileMode;
        fileMode.fill('0');
        fileMode.width(7);
        fileMode << right << oct << 0644;
        fileMode.get(recordHeader.mode, 8);

        stringstream size;
        size.fill('0');
        size.width(11);
        size << right << oct << byteLength;
        size.get(recordHeader.size, 12);

        stringstream timeStream;
        timeStream.fill('0');
        timeStream.width(11);
        timeStream << right << oct << time(NULL);
        timeStream.get(recordHeader.mtime, 12);
        // string("00000000000").copy(recordHeader.mtime, 12);

        // string("0000000").copy(recordHeader.devmajor, 8);
        // string("0000000").copy(recordHeader.devminor, 8);

        recordHeader.typeflag[0] = '0';

        recordHeader.version[0] = '0';
        recordHeader.version[1] = '0';

        // checksum is computed while the value in the checksum field
        // is all spaces (but it is null-terminated)
        for(size_t i(0); i < 8; ++i)
            recordHeader.chksum[i] = ' ';

        unsigned int checksum(0);
        for(size_t i(0); i < 512; ++i)
        {
            checksum += ((char*) &recordHeader)[i];
        }
        stringstream checksumStream;
        checksumStream.fill('0');
        checksumStream.width(6);
        checksumStream << right << oct << checksum;
        checksumStream.width(0);
        checksumStream << '\0' << ' ';
        checksumStream.get(recordHeader.chksum, 8);

        m_file.write((const char*) &recordHeader, sizeof(TarHeader));
        m_file.write((const char*) contents, byteLength);

        // pad all records up to 512 bytes
        if(byteLength % 512)
            for(size_t i(byteLength % 512); i < 512; ++i)
                m_file.put('\0');
    }

    SharedArray<char> TarArchive::read(const std::string &path)
    {
        return SharedArray<char>();
    }

    unsigned int TarArchive::size()
    {
        return 0;
    }

    string TarArchive::getItemName(unsigned int index)
    {
        return string();
    }

}
