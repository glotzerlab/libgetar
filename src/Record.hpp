// Record.hpp
// by Matthew Spellings <mspells@umich.edu>

#include <algorithm>
#include <string>
#include <vector>

#ifndef __RECORD_HPP_
#define __RECORD_HPP_

namespace gtar{

    using std::string;
    using std::vector;

    enum Behavior {Constant, Discrete, Continuous};

    enum Format {Float32, Float64, Int32, Int64, UInt8, UInt32, UInt64};

    enum Resolution {Text, Uniform, Individual};

    class Record
    {
    public:
        Record(const string &path);

        Record(const string &group, const string &name, const string &index,
               const string &suffix, Behavior behavior, Format format,
               Resolution resolution);

        Record(const Record &rhs);

        void operator=(const Record &rhs);
        bool operator==(const Record &rhs) const;
        bool operator!=(const Record &rhs) const;
        bool operator<(const Record &rhs) const;

        Record withNullifiedIndex() const;

    private:
        void process(const string &target, size_t start);
        void processVars(const string &target, size_t start);
        void processFrames(const string &target, size_t start);
        void processName(const string &target, size_t start);
        void processVarIdx(const string &target, size_t start);

        string m_group;
        string m_name;
        string m_index;
        string m_suffix;
        Behavior m_behavior;
        Format m_format;
        Resolution m_resolution;
    };

}
#endif
