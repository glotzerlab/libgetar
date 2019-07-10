// DirArchive.cpp
// by Matthew Spellings <mspells@umich.edu>

#include <cstring>
#include <cerrno>
#include <ctime>
#include <sys/types.h>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <sys/stat.h>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#include "../dirent/include/dirent.h"
#include <direct.h>
#else
#include <dirent.h>
#endif

#include "DirArchive.hpp"

#ifdef GTAR_NAMESPACE_PARENT
namespace GTAR_NAMESPACE_PARENT{
#endif

namespace gtar{

    using std::ios;
    using std::ios_base;
    using std::map;
    using std::oct;
    using std::right;
    using std::runtime_error;
    using std::fstream;
    using std::string;
    using std::stringstream;
    using std::vector;

    void createDirectory(string dir)
    {
        #if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
        _mkdir(dir.c_str());
        #else
        mkdir(dir.c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
        #endif
    }

    DirArchive::DirArchive(const string &filename, const OpenMode mode):
        m_filename(filename), m_mode(mode), m_createdDirectories()
    {
        // make sure that the base directory exists
        if(m_mode != Read)
        {
            struct stat dirStat;
            const int result(stat(filename.c_str(), &dirStat));
            if(result == 0) // found that name
            {
                // not a directory
                if(!(dirStat.st_mode & S_IFDIR))
                {
                    stringstream msg;
                    msg << "Error opening directory for write (file already exists)";
                    throw runtime_error(msg.str());
                }
            }
            else
                createDirectory(m_filename);
        }

        // populate the list of found file names. first, strip all
        // trailing slashes; this will never return npos because the
        // directory always has at least one slash
        const size_t stripLength(m_filename.find_last_not_of('/') + 1);
        const string stripped(m_filename.substr(0, stripLength));
        searchDirectory(stripped);
    }

    DirArchive::~DirArchive()
    {
    }

    void DirArchive::close()
    {
    }

    void DirArchive::writePtr(const string &path, const void *contents,
                              const size_t byteLength, CompressMode mode,
                              bool immediate)
    {
        if(m_mode == Read)
            throw runtime_error("Can't write to an archive opened for reading");

        for(size_t i(path.find('/', 0)); i != string::npos; i = path.find('/', i + 1))
        {
            const string segment(path.substr(0, i));
            if(m_createdDirectories.find(segment) == m_createdDirectories.end())
            {
                createDirectory(m_filename + segment);
                m_createdDirectories.insert(segment);
            }
        }

        fstream file((m_filename + path).c_str(), ios::binary | ios_base::out | ios_base::trunc);

        if(!file.good())
        {
            stringstream msg;
            msg << "Error opening path for writing: " << strerror(errno);
            throw runtime_error(msg.str());
        }

        file.write((const char*) contents, byteLength);
        file.close();

        m_fileNames.push_back(path);
    }

    void DirArchive::beginBulkWrites()
    {
    }

    void DirArchive::endBulkWrites()
    {
    }

    SharedArray<char> DirArchive::read(const std::string &path)
    {
        fstream file((m_filename + path).c_str(), ios_base::in);

        if(!file.good())
            return SharedArray<char>();

        std::streampos size(0);
        size = file.tellg();
        file.seekg(0, std::ios::end);
        size = file.tellg() - size;
        file.seekg(0);

        SharedArray<char> result(new char[size], size);

        file.read(result.get(), size);
        file.close();

        return result;
    }

    unsigned int DirArchive::size()
    {
        return m_fileNames.size();
    }

    string DirArchive::getItemName(unsigned int index)
    {
        return m_fileNames[index];
    }

    void DirArchive::searchDirectory(const string &path)
    {
        DIR *curDir(opendir(path.c_str()));
        if(curDir == NULL)
        {
            stringstream msg;
            msg << "Error opening directory " << path;
            msg << ": " << strerror(errno);
            throw runtime_error(msg.str());
        }

        dirent *curEnt(readdir(curDir));
        struct stat curStat;

        while(curEnt != NULL)
        {
            if(strcmp(curEnt->d_name, ".") && strcmp(curEnt->d_name, ".."))
            {
                stringstream fname;
                fname << path << '/' << curEnt->d_name;
                const string entName(fname.str());
                stat(entName.c_str(), &curStat);
                // is a directory
                if(curStat.st_mode & S_IFDIR)
                    searchDirectory(entName);
                // only grab regular files
                else if(curStat.st_mode & S_IFREG)
                    m_fileNames.push_back(entName);
            }
            curEnt = readdir(curDir);
        }

        closedir(curDir);
    }
}

#ifdef GTAR_NAMESPACE_PARENT
}
#endif
