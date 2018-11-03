// GTAR.cpp
// by Matthew Spellings <mspells@umich.edu>

#include "GTAR.hpp"
#include "SharedArray.hpp"

#include <algorithm>
#include <stdexcept>
#include <stdint.h>
#include <sys/stat.h>

#ifdef GTAR_NAMESPACE_PARENT
namespace GTAR_NAMESPACE_PARENT{
#endif

namespace gtar{

    using std::map;
    using std::runtime_error;
    using std::set;
    using std::string;
    using std::swap;
    using std::vector;

    bool littleEndian()
    {
        int x(1);
        return *(char*)&x;
    }

    bool IndexCompare::operator()(const string &a, const string &b) const
    {
        return a.size() < b.size() || ((a.size() == b.size()) && a < b);
    }

    GTAR::BulkWriter::BulkWriter(GTAR &archive):
        m_archive(archive)
    {
        m_archive.beginBulkWrites();
    }

    GTAR::BulkWriter::~BulkWriter()
    {
        m_archive.endBulkWrites();
    }

    void GTAR::BulkWriter::writeString(const string &path, const string &contents,
                                       CompressMode mode)
    {
        m_archive.writeString(path, contents, mode, false);
    }

    void GTAR::BulkWriter::writeBytes(const string &path, const vector<char> &contents,
                                      CompressMode mode)
    {
        m_archive.writeBytes(path, contents, mode, false);
    }

    void GTAR::BulkWriter::writePtr(const string &path, const void *contents,
                                    const size_t byteLength, CompressMode mode)
    {
        m_archive.writePtr(path, contents, byteLength, mode, false);
    }

    GTAR::GTAR(const string &filename, const OpenMode mode):
        m_archive(), m_records(), m_indexedRecords()
    {
        OpenMode realMode(mode);

        if(mode == Append)
        {
            bool fileExists(false);
            struct stat fileStat;
            fileExists = stat(filename.c_str(), &fileStat) == 0;

            if(!fileExists)
                realMode = Write;
        }

        if(filename.length() >= 4 && filename.rfind(".tar") == filename.length() - 4)
            m_archive.reset(new TarArchive(filename, realMode));
        else if(filename.length() >= 1 && filename.rfind("/") == filename.length() - 1)
            m_archive.reset(new DirArchive(filename, realMode));
        else if(filename.length() >= 7 && filename.rfind(".sqlite") == filename.length() - 7)
        {
            m_archive.reset(new SqliteArchive(filename, realMode));
        }
        else
            m_archive.reset(new ZipArchive(filename, realMode));

        // Populate our record list unconditionally
        const unsigned int size(m_archive->size());
        for(unsigned int index(0); index < size; ++index)
            insertRecord(m_archive->getItemName(index));
    }

    void GTAR::close()
    {
        m_archive.reset();
    }

    void GTAR::writeString(const string &path, const string &contents,
                           CompressMode mode)
    {
        writeString(path, contents, mode, true);
    }

    void GTAR::writeBytes(const string &path, const vector<char> &contents,
                          CompressMode mode)
    {
        writeBytes(path, contents, mode, true);
    }

    void GTAR::writePtr(const string &path, const void *contents,
                        const size_t byteLength, CompressMode mode)
    {
        writePtr(path, contents, byteLength, mode, true);
    }

    void GTAR::writeString(const string &path, const string &contents,
                           CompressMode mode, bool immediate)
    {
        if(m_archive.get())
        {
            m_archive->writePtr(path, contents.data(), contents.size(), mode, immediate);
            insertRecord(path);
        }
        else
            throw runtime_error("Calling writeString() with a closed GTAR object");
    }

    void GTAR::writeBytes(const string &path, const vector<char> &contents,
                          CompressMode mode, bool immediate)
    {
        if(m_archive.get())
        {
            m_archive->writeVec(path, contents, mode, immediate);
            insertRecord(path);
        }
        else
            throw runtime_error("Calling writeBytes() with a closed GTAR object");
    }

    void GTAR::writePtr(const string &path, const void *contents,
                        const size_t byteLength, CompressMode mode, bool immediate)
    {
        if(m_archive.get())
        {
            m_archive->writePtr(path, contents, byteLength, mode, immediate);
            insertRecord(path);
        }
        else
            throw runtime_error("Calling writePtr() with a closed GTAR object");
    }

    void GTAR::beginBulkWrites()
    {
        if(m_archive.get())
            m_archive->beginBulkWrites();
        else
            throw runtime_error("Calling beginBulkWrites() with a closed GTAR object");
    }

    void GTAR::endBulkWrites()
    {
        if(m_archive.get())
            m_archive->endBulkWrites();
        else
            throw runtime_error("Calling endBulkWrites() with a closed GTAR object");
    }

    SharedArray<char> GTAR::readBytes(const string &path)
    {
        if(m_archive.get())
            return m_archive->read(path);
        else
            throw runtime_error("Calling readBytes() with a closed GTAR object");
    }

    vector<Record> GTAR::getRecordTypes() const
    {
        vector<Record> result;

        for(map<Record, indexSet>::const_iterator iter(m_records.begin());
            iter != m_records.end(); ++iter)
            result.push_back(iter->first);

        return result;
    }

    vector<string> GTAR::queryFrames(const Record &target) const
    {
        Record query(target.withNullifiedIndex());
        map<Record, indexSet>::const_iterator result(m_records.find(query));

        if(result != m_records.end())
            return vector<string>(result->second.begin(), result->second.end());

        return vector<string>();
    }

    void GTAR::insertRecord(const string &path)
    {
        Record rec(path);
        const string index(rec.nullifyIndex());

        m_records[rec].insert(index);
        m_indexedRecords[rec].push_back(index);
    }

}

#ifdef GTAR_NAMESPACE_PARENT
}
#endif
