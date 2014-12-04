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

    // Time behavior of properties
    enum Behavior {Constant, Discrete, Continuous};

    // Binary formats in which properties can be stored
    enum Format {Float32, Float64, Int32, Int64, UInt8, UInt32, UInt64};

    // Level of detail of property storage
    enum Resolution {Text, Uniform, Individual};

    // Simple class for a record which can be stored in an archive
    class Record
    {
    public:
        // Default constructor: initialize all strings to empty,
        // behavior to Constant, format to UInt8, and resolution to
        // Text
        Record();

        // Create a record from a path (inside the archive), parsing
        // the path into the various fields
        Record(const string &path);

        // Create a record directly from the full set of elements
        Record(const string &group, const string &name, const string &index,
               const string &suffix, Behavior behavior, Format format,
               Resolution resolution);

        // Copy constructor
        Record(const Record &rhs);

        // Assignment operator
        void operator=(const Record &rhs);

        // Comparison operators
        bool operator==(const Record &rhs) const;
        bool operator!=(const Record &rhs) const;
        bool operator<(const Record &rhs) const;

        // Copy all fields from rhs into this object
        void copy(const Record &rhs);

        // Set our index to the empty string
        string nullifyIndex();
        // Return a copy of this object, but with an empty string for
        // its index
        Record withNullifiedIndex() const;

        // Construct a path (for inside an archive) from this object's
        // various fields
        string getPath() const;

        // Access fields of this object
        string getGroup() const;
        string getName() const;
        string getIndex() const;
        Format getFormat() const;
        Resolution getResolution() const;
        void setIndex(const string &index);

    private:
        // Parse the given target path inside an archive in various
        // stages
        void process(const vector<string> &dirs);
        void processVarIndex(const string &index);
        void processFrameIndex(const string &index);
        void processName(const string &name);

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
