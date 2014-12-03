// Record.cpp
// by Matthew Spellings <mspells@umich.edu>

#include "Record.hpp"

#include <sstream>

namespace gtar{
    using std::string;
    using std::stringstream;
    using std::vector;

    Record::Record():
        m_group(), m_name(), m_index(), m_suffix(), m_behavior(Constant),
        m_format(UInt8), m_resolution(Text)
    {
    }

    Record::Record(const string &path):
        m_group(), m_name(), m_index(), m_suffix(), m_behavior(Constant),
        m_format(UInt8), m_resolution(Text)
    {
        process(path, 0);
    }

    Record::Record(const string &group, const string &name, const string &index,
                   const string &suffix, Behavior behavior, Format format,
                   Resolution resolution):
        m_group(group), m_name(name), m_index(index), m_suffix(suffix),
        m_behavior(behavior), m_format(format), m_resolution(resolution)
    {}

    Record::Record(const Record &rhs):
        m_group(rhs.m_group), m_name(rhs.m_name), m_index(rhs.m_index), m_suffix(rhs.m_suffix),
        m_behavior(rhs.m_behavior), m_format(rhs.m_format), m_resolution(rhs.m_resolution)
    {}

    void Record::operator=(const Record &rhs)
    {
        m_group = rhs.m_group;
        m_name = rhs.m_name;
        m_index = rhs.m_index;
        m_suffix = rhs.m_suffix;
        m_behavior = rhs.m_behavior;
        m_format = rhs.m_format;
        m_resolution = rhs.m_resolution;
    }

    bool Record::operator==(const Record &rhs) const
    {
        return m_group == rhs.m_group && m_name == rhs.m_name &&
            m_index == rhs.m_index && m_suffix == rhs.m_suffix &&
            m_behavior == rhs.m_behavior && m_format == rhs.m_format &&
            m_resolution == rhs.m_resolution;
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
               lthelper(m_suffix, rhs.m_suffix,
               lthelper(m_behavior, rhs.m_behavior,
               lthelper(m_format, rhs.m_format,
               lthelper(m_resolution, rhs.m_resolution,
                        m_index < rhs.m_index))))));
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
        default:
            fullName << m_suffix;
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

    string Record::getName() const
    {
        return m_name;
    }

    string Record::getIndex() const
    {
        return m_index;
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

    void Record::process(const string &target, size_t start)
    {
        if(start == target.size()) return;

        const size_t firstSlash(target.find('/', start));

        if(firstSlash != string::npos)
        {
            if(firstSlash - start == 6 &&
               target.compare(start, 6, "frames") == 0)
                processFrames(target, firstSlash + 1);
            else if(firstSlash - start == 4 &&
                    target.compare(start, 4, "vars") == 0)
                processVars(target, firstSlash + 1);
            else
            {
                if(m_group.size())
                    m_group += '/';
                m_group += target.substr(start, firstSlash - start);
                process(target, firstSlash + 1);
            }
        }
        else
        {
            processName(target, start);
        }
    }

    void Record::processVars(const string &target, size_t start)
    {
        if(start == target.size()) return;

        m_behavior = Continuous;

        const size_t firstSlash(target.find('/', start));

        if(firstSlash != string::npos)
        {
            m_name = target.substr(start, firstSlash - start);
            processVarIdx(target, firstSlash + 1);
        }
    }

    void Record::processFrames(const string &target, size_t start)
    {
        if(start == target.size()) return;

        m_behavior = Discrete;

        const size_t firstSlash(target.find('/', start));

        if(firstSlash != string::npos)
        {
            m_index = target.substr(start, firstSlash - start);
            processName(target, firstSlash + 1);
        }
    }

    void Record::processName(const string &target, size_t start)
    {
        if(start == target.size()) return;

        const size_t lastComponent(target.rfind('.'));

        if(lastComponent != string::npos && lastComponent > 0)
        {
            const size_t interComponent(target.rfind('.', lastComponent - 1));
            const size_t delta(lastComponent - interComponent);

            if(interComponent != string::npos && (delta == 4 || delta == 3))
            {
                m_name += target.substr(start, interComponent - start);

                if(target.compare(interComponent, 4, ".f32") == 0)
                    m_format = Float32;
                else if(target.compare(interComponent, 4, ".f64") == 0)
                    m_format = Float64;
                else if(target.compare(interComponent, 4, ".i32") == 0)
                    m_format = Int32;
                else if(target.compare(interComponent, 4, ".i64") == 0)
                    m_format = Int64;
                else if(target.compare(interComponent, 3, ".u8") == 0)
                    m_format = UInt8;
                else if(target.compare(interComponent, 4, ".u32") == 0)
                    m_format = UInt32;
                else if(target.compare(interComponent, 4, ".u64") == 0)
                    m_format = UInt64;
                else
                    m_name += target.substr(interComponent, delta);

                if(target.compare(lastComponent, 4, ".ind") == 0)
                    m_resolution = Individual;
                else if(target.compare(lastComponent, 4, ".uni") == 0)
                    m_resolution = Uniform;
                else
                    m_name += target.substr(lastComponent);
            }
            else
            {
                m_name = target.substr(start);
                m_format = UInt8;
                m_resolution = Text;
            }
        }
    }

    void Record::processVarIdx(const string &target, size_t start)
    {
        if(start == target.size()) return;

        const size_t firstComponent(target.find('.', start));

        if(firstComponent != string::npos)
        {
            m_index = target.substr(start, firstComponent - start);
            m_suffix = target.substr(firstComponent);
        }
        else
            m_suffix = target.substr(start);
    }
}
