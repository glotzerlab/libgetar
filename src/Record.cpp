// Record.cpp
// by Matthew Spellings <mspells@umich.edu>

#include "Record.hpp"

namespace gtar{
    using std::string;
    using std::vector;

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

    bool Record::operator<(const Record &rhs) const
    {
        return m_group < rhs.m_group || m_name < rhs.m_name ||
             m_suffix < rhs.m_suffix || m_behavior < rhs.m_behavior ||
             m_format < rhs.m_format || m_resolution < rhs.m_resolution ||
             m_index < rhs.m_index;
    }

    Record Record::withNullifiedIndex() const
    {
        Record result(*this);
        result.m_index = string();
        return result;
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
