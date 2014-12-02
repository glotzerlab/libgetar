// GTAR.cpp
// by Matthew Spellings <mspells@umich.edu>

#include "GTAR.hpp"
#include "SharedArray.hpp"

#include <algorithm>
#include <stdexcept>
#include <stdint.h>

namespace gtar{

    using std::map;
    using std::runtime_error;
    using std::set;
    using std::string;
    using std::swap;
    using std::vector;

    bool IndexCompare::operator()(const string &a, const string &b)
    {
        return a.size() < b.size() || ((a.size() == b.size()) && a < b);
    }

    GTAR::GTAR(const string &filename, const OpenMode mode):
        m_archive(), m_records(), m_indexedRecords()
    {
        if(filename.rfind(".tar") == filename.length() - 4)
            m_archive.reset(new TarArchive(filename, mode));
        else
            m_archive.reset(new ZipArchive(filename, mode));

        // Populate our record list
        if(mode == Read)
        {
            const unsigned int size(m_archive->size());
            for(unsigned int index(0); index < size; ++index)
                insertRecord(m_archive->getItemName(index));
        }
    }

    void GTAR::close()
    {
        m_archive.reset();
    }

    void GTAR::writeString(const string &path, const string &contents, CompressMode mode)
    {
        if(m_archive.get())
            m_archive->writePtr(path, contents.data(), contents.size(), mode);
        else
            throw runtime_error("Calling writeString() with a closed GTAR object");
    }

    void GTAR::writeBytes(const string &path, const vector<char> &contents, CompressMode mode)
    {
        if(m_archive.get())
            m_archive->writeVec(path, contents, mode);
        else
            throw runtime_error("Calling writeBytes() with a closed GTAR object");
    }

    void GTAR::writePtr(const string &path, const void *contents,
                        const size_t byteLength, CompressMode mode)
    {
        if(m_archive.get())
            m_archive->writePtr(path, contents, byteLength, mode);
        else
            throw runtime_error("Calling writePtr() with a closed GTAR object");
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
