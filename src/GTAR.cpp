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

    GTAR::GTAR(const string &filename, const OpenMode mode):
        m_archive(filename, mode), m_records(), m_indexedRecords()
    {
        // Populate our record list
        if(mode == Read)
        {
            const unsigned int size(m_archive.size());
            for(unsigned int index(0); index < size; ++index)
                insertRecord(m_archive.getItemName(index));
        }
    }

    void GTAR::writeString(const string &path, const string &contents, CompressMode mode)
    {
        m_archive.writePtr(path, contents.data(), contents.size(), mode);
    }

    void GTAR::writeBytes(const string &path, const vector<char> &contents, CompressMode mode)
    {
        m_archive.writeVec(path, contents, mode);
    }

    void GTAR::writePtr(const string &path, const void *contents,
                        const size_t byteLength, CompressMode mode)
    {
        m_archive.writePtr(path, contents, byteLength, mode);
    }

    SharedArray<char> GTAR::readBytes(const string &path)
    {
        return m_archive.read(path);
    }

    unsigned int GTAR::queryRecordCount(const Record &rec)
    {
        const Record query(rec.withNullifiedIndex());

        if(m_records.find(query) != m_records.end())
            return m_records[rec].size();

        return 0;
    }

    string GTAR::getRecordIndex(const Record &rec, unsigned int index)
    {
        Record query(rec.withNullifiedIndex());

        if(m_indexedRecords.find(query) != m_indexedRecords.end() &&
           index < m_indexedRecords[rec].size())
            return m_indexedRecords[rec][index];

        return string();
    }

    void GTAR::insertRecord(const string &path)
    {
        Record rec(path);
        const string index(rec.nullifyIndex());

        if(m_records.find(rec) == m_records.end())
        {
            m_records[rec] = set<string>();
            m_indexedRecords[rec] = vector<string>();
        }

        m_records[rec].insert(index);
        m_indexedRecords[rec].push_back(index);
    }

}
