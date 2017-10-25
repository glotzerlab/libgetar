// TarArchive.cpp
// by Matthew Spellings <mspells@umich.edu>

#include <cstring>
#include <ctime>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include "TarArchive.hpp"

#ifdef GTAR_NAMESPACE_PARENT
namespace GTAR_NAMESPACE_PARENT{
#endif

namespace gtar{

    using std::ios_base;
    using std::map;
    using std::oct;
    using std::right;
    using std::runtime_error;
    using std::string;
    using std::stringstream;
    using std::vector;

    TarArchive::TarArchive(const string &filename, const OpenMode mode):
        m_filename(filename), m_mode(mode), m_file(), m_filePosition(0), m_maxPosition(0)
    {
        ios_base::openmode fileMode(ios_base::binary | ios_base::in);

        switch(mode)
        {
        case Write:
            fileMode |= (ios_base::out | ios_base::trunc);
            break;
        case Append:
            fileMode |= (ios_base::out);
            break;
        case Read:
        default:
            break;
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

        // populate the file location maps
        {
            bool done(false);
            size_t offset(0);
            TarHeader recordHeader;

            while(!m_file.eof() && !done)
            {
                memset(&recordHeader, 0, sizeof(TarHeader));
                m_file.read((char*) &recordHeader, sizeof(TarHeader));

                if(recordHeader.magic[0] == '\0')
                {
                    bool allZero(true);
                    // check if this record is all zero; if so, assume
                    // we're at the end of the file
                    for(size_t i(0); i < sizeof(TarHeader); ++i)
                    {
                        allZero &= ((char*) &recordHeader)[i] == '\0';
                    }

                    done |= allZero;
                }
                else if(strncmp("ustar", recordHeader.magic, 5))
                {
                    stringstream message;
                    message << "Error reading tar record at position " <<
                        offset << ": magic mismatch (is this actually a tar file?)";

                    // If this fails at offset 0, we must not have
                    // been given an actual tar archive; otherwise,
                    // finish reading immediately with whatever we've
                    // found so far
                    if(!offset)
                        throw runtime_error(message.str());
                    else
                    {
                        std::cerr << message.str() << std::endl;
                        done = true;
                    }
                }
                else
                {
                    string fileName(string(recordHeader.prefix) +
                                    string(recordHeader.name));

                    size_t size(0);
                    stringstream sizeStream;
                    sizeStream << recordHeader.size;
                    sizeStream >> oct >> size;

                    m_fileNames.push_back(fileName);
                    m_fileOffsets[fileName] = offset + sizeof(TarHeader);
                    m_fileSizes[fileName] = size;

                    offset += sizeof(TarHeader) + (size + 511)/512*512;

                    m_file.seekg(offset);
                }
            }

            m_file.clear();
            m_file.seekg(0);
        }

        if(m_fileNames.size())
        {
            const string lastName = m_fileNames[m_fileNames.size() - 1];
            m_maxPosition = m_fileOffsets[lastName] + (m_fileSizes[lastName] + 511)/512*512;
        }

        if(m_mode == Append)
        {
            m_file.seekp(m_maxPosition);
            m_filePosition = m_maxPosition;
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
            m_file.seekp(m_maxPosition);
            // pad the end of file with two 512B blocks
            for(size_t i(0); i < 1024; ++i)
                m_file.put('\0');
            m_file.close();
        }
    }

    void TarArchive::writePtr(const string &path, const void *contents,
                              const size_t byteLength, CompressMode mode,
                              bool immediate)
    {
        switch(m_mode)
        {
        case Read:
            throw runtime_error("Can't write to an archive opened for reading");
            break;
        case Append:
        case Write:
        default:
            // seek to end if we aren't already there
            if(m_filePosition != m_maxPosition)
            {
                m_file.seekp(m_maxPosition);
                m_filePosition = m_maxPosition;
            }
            break;
        }

        string prefix, name(path);

        const size_t savedOffset(m_maxPosition + (std::streampos) sizeof(TarHeader));

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

        if(immediate)
            endBulkWrites();

        const size_t deltaSize = sizeof(TarHeader) + (byteLength + 511)/512*512;
        m_filePosition += deltaSize;
        m_maxPosition += deltaSize;

        m_fileNames.push_back(path);
        m_fileOffsets[path] = savedOffset;
        m_fileSizes[path] = byteLength;
    }

    void TarArchive::beginBulkWrites()
    {
    }

    void TarArchive::endBulkWrites()
    {
        m_file.flush();
    }

    SharedArray<char> TarArchive::read(const std::string &path)
    {
        if(m_fileOffsets.find(path) == m_fileOffsets.end())
            return SharedArray<char>();

        m_file.seekg(m_fileOffsets[path]);

        const size_t size(m_fileSizes[path]);
        SharedArray<char> result(new char[size], size);

        m_file.read(result.get(), size);

        m_filePosition = m_fileOffsets[path] + size;

        return result;
    }

    unsigned int TarArchive::size()
    {
        return m_fileNames.size();
    }

    string TarArchive::getItemName(unsigned int index)
    {
        return m_fileNames[index];
    }

}

#ifdef GTAR_NAMESPACE_PARENT
}
#endif
