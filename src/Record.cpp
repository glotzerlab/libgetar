// Record.cpp
// by Matthew Spellings <mspells@umich.edu>

#include "Record.hpp"

#include <iostream>
using namespace std;

#include <sstream>

#ifdef GTAR_NAMESPACE_PARENT
namespace GTAR_NAMESPACE_PARENT{
#endif

namespace gtar{
    using std::max;
    using std::string;
    using std::stringstream;
    using std::vector;

    Record::Record():
        m_group(), m_name(), m_index(), m_behavior(Constant),
        m_format(UInt8), m_resolution(Text)
    {
    }

    Record::Record(const string &path):
        m_group(), m_name(), m_index(), m_behavior(Constant),
        m_format(UInt8), m_resolution(Text)
    {
        vector<string> dirs;

        for(size_t pos(0), nextpos(path.find('/', 0));
            pos != string::npos;
            pos = (nextpos == string::npos? nextpos: nextpos + 1), nextpos = path.find('/', pos))
        {
            if(nextpos == string::npos)
                dirs.push_back(path.substr(pos));
            else
                dirs.push_back(path.substr(pos, nextpos - pos));
        }

        process(dirs);
    }

    Record::Record(const string &group, const string &name, const string &index,
                   Behavior behavior, Format format, Resolution resolution):
        m_group(group), m_name(name), m_index(index),
        m_behavior(behavior), m_format(format), m_resolution(resolution)
    {}

    Record::Record(const Record &rhs):
        m_group(rhs.m_group), m_name(rhs.m_name), m_index(rhs.m_index),
        m_behavior(rhs.m_behavior), m_format(rhs.m_format), m_resolution(rhs.m_resolution)
    {}

    void Record::operator=(const Record &rhs)
    {
        m_group = rhs.m_group;
        m_name = rhs.m_name;
        m_index = rhs.m_index;
        m_behavior = rhs.m_behavior;
        m_format = rhs.m_format;
        m_resolution = rhs.m_resolution;
    }

    bool Record::operator==(const Record &rhs) const
    {
        return m_group == rhs.m_group && m_name == rhs.m_name &&
            m_index == rhs.m_index && m_behavior == rhs.m_behavior &&
            m_format == rhs.m_format && m_resolution == rhs.m_resolution;
    }

    bool Record::operator!=(const Record &rhs) const
    {
        return !(*this == rhs);
    }

    template<typename T>
    bool lthelper(T a, T b, bool rest)
    {
        return a < b || ((a == b) && rest);
    }

    bool Record::operator<(const Record &rhs) const
    {
        return lthelper(m_group, rhs.m_group,
               lthelper(m_name, rhs.m_name,
               lthelper(m_behavior, rhs.m_behavior,
               lthelper(m_format, rhs.m_format,
               lthelper(m_resolution, rhs.m_resolution,
                        m_index < rhs.m_index)))));
    }

    void Record::copy(const Record &rhs)
    {
        *this = rhs;
    }

    string Record::nullifyIndex()
    {
        string result;
        m_index.swap(result);
        return result;
    }

    Record Record::withNullifiedIndex() const
    {
        Record result(*this);
        result.nullifyIndex();
        return result;
    }

    string Record::getPath() const
    {
        stringstream fullName;
        fullName << m_name;

        if(m_resolution != Text)
        {
            switch(m_format)
            {
            case Float32:
                fullName << ".f32";
                break;
            case Float64:
                fullName << ".f64";
                break;
            case Int32:
                fullName << ".i32";
                break;
            case Int64:
                fullName << ".i64";
                break;
            case UInt32:
                fullName << ".u32";
                break;
            case UInt64:
                fullName << ".u64";
                break;
            case UInt8:
            default:
                fullName << ".u8";
            }
        }

        switch(m_resolution)
        {
        case Uniform:
            fullName << ".uni";
            break;
        case Individual:
            fullName << ".ind";
            break;
        case Text:
            // not handled
            break;
        }

        stringstream result;

        if(m_group.size())
            result << m_group << '/';

        switch(m_behavior)
        {
        case Continuous:
            result << "vars/" << fullName.str() << '/' << m_index;
            break;
        case Discrete:
            result << "frames/" << m_index << '/' << fullName.str();
            break;
        case Constant:
        default:
            result << fullName.str();
        }

        return result.str();
    }

    string Record::getGroup() const
    {
        return m_group;
    }

    string Record::getName() const
    {
        return m_name;
    }

    string Record::getIndex() const
    {
        return m_index;
    }

    Behavior Record::getBehavior() const
    {
        return m_behavior;
    }

    Format Record::getFormat() const
    {
        return m_format;
    }

    Resolution Record::getResolution() const
    {
        return m_resolution;
    }

    void Record::setIndex(const string &index)
    {
        m_index = index;
    }

    void Record::process(const vector<string> &dirs)
    {
        // All but the last 3 segments of the path definitely go into
        // m_group
        for(size_t i(3); i < dirs.size(); ++i)
        {
            if(m_group.size())
                m_group += '/';
            m_group += dirs[i - 3];
        }

        if(dirs.size() > 2)
        {
            if(dirs[dirs.size() - 3] == "frames")
            {
                processFrameIndex(dirs[dirs.size() - 2]);
                processName(dirs[dirs.size() - 1]);
                return;
            }
            else if(dirs[dirs.size() - 3] == "vars")
            {
                processName(dirs[dirs.size() - 2]);
                processVarIndex(dirs[dirs.size() - 1]);
                return;
            }
        }

        // We didn't find frames/ or vars/; add everything up to the
        // last segment to m_group
        for(size_t i(max(0, (int) dirs.size() - 3)); i + 1 < dirs.size(); ++i)
        {
            if(m_group.size())
                m_group += '/';
            m_group += dirs[i];
        }

        if(dirs.size())
            processName(dirs[dirs.size() - 1]);
    }

    void Record::processVarIndex(const string &index)
    {
        m_behavior = Continuous;
        m_index = index;
    }

    void Record::processFrameIndex(const string &index)
    {
        m_behavior = Discrete;
        m_index = index;
    }

    void Record::processName(const string &name)
    {
        vector<string> pieces;

        for(size_t pos(0), nextpos(name.find('.', 0));
            pos != string::npos;
            pos = (nextpos == string::npos? nextpos: nextpos + 1), nextpos = name.find('.', pos))
        {
            if(nextpos == string::npos)
                pieces.push_back(name.substr(pos));
            else
                pieces.push_back(name.substr(pos, nextpos - pos));
        }

        // All but the last 3 pieces of the name definitely go into
        // m_name
        for(size_t i(3); i < pieces.size(); ++i)
        {
            m_name += pieces[i - 3];
            m_name += '.';
        }

        if(pieces.size() > 2)
        {
            m_name += pieces[pieces.size() - 3];

            if(pieces[pieces.size() - 1] == "ind" ||
               pieces[pieces.size() - 1] == "uni")
            {
                // this will be overwritten if we fail to parse a
                // proper format in the block below
                if(pieces[pieces.size() - 1] == "ind")
                    m_resolution = Individual;
                else if(pieces[pieces.size() - 1] == "uni")
                    m_resolution = Uniform;

                if(pieces[pieces.size() - 2] == "f32")
                    m_format = Float32;
                else if(pieces[pieces.size() - 2] == "f64")
                    m_format = Float64;
                else if(pieces[pieces.size() - 2] == "i32")
                    m_format = Int32;
                else if(pieces[pieces.size() - 2] == "i64")
                    m_format = Int64;
                else if(pieces[pieces.size() - 2] == "u8")
                    m_format = UInt8;
                else if(pieces[pieces.size() - 2] == "u32")
                    m_format = UInt32;
                else if(pieces[pieces.size() - 2] == "u64")
                    m_format = UInt64;
                else
                {
                    m_name += '.';
                    m_name += pieces[pieces.size() - 2];
                    m_name += '.';
                    m_name += pieces[pieces.size() - 1];

                    m_format = UInt8;
                    m_resolution = Text;
                }

            }
            else
            {
                m_name += '.';
                m_name += pieces[pieces.size() - 2];
                m_name += '.';
                m_name += pieces[pieces.size() - 1];

                m_format = UInt8;
                m_resolution = Text;
            }

        }
        else
        {
            for(size_t i(0); i < pieces.size(); ++i)
            {
                m_name += pieces[i];
                if(i + 1 < pieces.size())
                    m_name += '.';
            }
        }
    }
}

#ifdef GTAR_NAMESPACE_PARENT
}
#endif
