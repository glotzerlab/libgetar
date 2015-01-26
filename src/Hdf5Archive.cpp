// Hdf5Archive.cpp
// by Matthew Spellings <mspells@umich.edu>

#include <memory>
#include <sstream>
#include <stdexcept>

#ifdef ENABLE_HDF5

#include "SharedArray.hpp"
#include "Hdf5Archive.hpp"
#include <H5Cpp.h>
#include <iostream>

namespace gtar{

    using std::auto_ptr;
    using std::runtime_error;
    using std::string;
    using std::stringstream;
    using std::vector;
    using H5::DataSet;
    using H5::DataSpace;
    using H5::Group;
    using H5::H5File;
    using H5::PredType;

    vector<string> splitPath(const string &path)
    {
        stringstream stream(path);
        vector<string> result;
        string cur;

        while(std::getline(stream, cur, '/'))
            result.push_back(cur);

        return result;
    }

    Hdf5Archive::Hdf5Archive(const string &filename, const OpenMode mode):
        m_filename(filename), m_mode(mode), m_archive()
    {
        if(m_mode == Write)
        {
            m_archive = H5File(m_filename.c_str(), H5F_ACC_TRUNC);
        }
        else if(m_mode == Read)
        {
            m_archive = H5File(m_filename.c_str(), H5F_ACC_RDONLY);
        }
        else if(m_mode == Append)
        {
            m_archive = H5File(m_filename.c_str(), H5F_ACC_RDWR);
        }

        H5::FileIException::dontPrint();
        H5::GroupIException::dontPrint();
    }

    Hdf5Archive::~Hdf5Archive()
    {
        close();
    }

    void Hdf5Archive::close()
    {
        m_archive.close();
    }

    void Hdf5Archive::writePtr(const string &path, const void *contents,
                           const size_t byteLength, CompressMode mode)
    {
        if(m_mode == Read)
            throw runtime_error("Can't write to an archive opened for reading");

        unsigned int h5type(0);
        hsize_t h5size[] = {byteLength};
        DataSet dset;
        vector<string> pathComponents(splitPath(path));
        H5::CommonFG *currentGroup(&m_archive);
        auto_ptr<H5::CommonFG> tempGroup;

        const string filename(pathComponents.back());
        pathComponents.pop_back();

        for(vector<string>::const_iterator seg(pathComponents.begin());
            seg != pathComponents.end(); ++seg)
        {
            try
            {
                tempGroup.reset(new Group(currentGroup->openGroup(*seg)));
            }
            catch(H5::FileIException notFoundError)
            {
                tempGroup.reset(new Group(currentGroup->createGroup(*seg)));
            }
            catch(H5::GroupIException notFoundError)
            {
                tempGroup.reset(new Group(currentGroup->createGroup(*seg)));
            }
            currentGroup = tempGroup.get();
        }

        try
        {
            dset = currentGroup->openDataSet(filename);
            dset.close();
            currentGroup->unlink(filename);
        }
        catch(H5::FileIException notFoundError)
        {
            // it wasn't found, so we don't have to remove it
        }
        catch(H5::GroupIException notFoundError)
        {
            // it wasn't found, so we don't have to remove it
        }

        dset = currentGroup->createDataSet(
            filename, PredType::STD_U8LE, DataSpace(1, h5size));

        dset.write(contents, PredType::STD_U8LE);
    }

    SharedArray<char> Hdf5Archive::read(const string &path)
    {
        if(m_mode != Read)
            throw runtime_error("Can't read from a file not opened for reading");

        try
        {
            DataSet dset(m_archive.openDataSet(path.c_str()));

            SharedArray<char> result(new char[dset.getStorageSize()], dset.getStorageSize());

            dset.read(result.get(), PredType::STD_U8LE);

            return result;
        }
        catch(H5::FileIException notFoundError)
        {
            // it wasn't found; return an empty SharedArray
        }

        return SharedArray<char>();
    }

    unsigned int Hdf5Archive::size()
    {
        return m_archive.getNumObjs();
    }

    string Hdf5Archive::getItemName(unsigned int index)
    {
        if(index < this->size())
            return m_archive.getObjnameByIdx(index);
        else
            return string();
    }
}

#endif
