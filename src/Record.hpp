// Record.hpp
// by Matthew Spellings <mspells@umich.edu>

#include <algorithm>
#include <string>
#include <vector>

#ifndef __RECORD_HPP_
#define __RECORD_HPP_

#ifdef GTAR_NAMESPACE_PARENT
namespace GTAR_NAMESPACE_PARENT{
#endif

namespace gtar{

    /// Time behavior of properties
    enum Behavior {Constant, Discrete, Continuous};

    /// Binary formats in which properties can be stored
    enum Format {Float32, Float64, Int32, Int64, UInt8, UInt32, UInt64};

    /// Level of detail of property storage
    enum Resolution {Text, Uniform, Individual};

    /// Simple class for a record which can be stored in an archive
    class Record
    {
    public:
        /// Default constructor: initialize all strings to empty,
        /// behavior to Constant, format to UInt8, and resolution to
        /// Text
        Record();

        /// Create a record from a path (inside the archive), parsing
        /// the path into the various fields
        Record(const std::string &path);

        /// Create a record directly from the full set of elements
        Record(const std::string &group, const std::string &name, const std::string &index,
               Behavior behavior, Format format, Resolution resolution);

        /// Copy constructor
        Record(const Record &rhs);

        /// Assignment operator
        void operator=(const Record &rhs);

        /// Equality
        bool operator==(const Record &rhs) const;
        /// Inequality
        bool operator!=(const Record &rhs) const;
        /// Comparison
        bool operator<(const Record &rhs) const;

        /// Copy all fields from rhs into this object
        void copy(const Record &rhs);

        /// Set our index to the empty string
        std::string nullifyIndex();
        /// Return a copy of this object, but with an empty string for
        /// its index
        Record withNullifiedIndex() const;

        /// Construct a path (for inside an archive) from this object's
        /// various fields
        std::string getPath() const;

        /// Get the stored group field
        std::string getGroup() const;
        /// Get the stored name field
        std::string getName() const;
        /// Get the stored index field
        std::string getIndex() const;
        /// Get the stored behavior field
        Behavior getBehavior() const;
        /// Get the stored format field
        Format getFormat() const;
        /// Get the stored resolution field
        Resolution getResolution() const;

        /// Set the index field for this Record object
        void setIndex(const std::string &index);

    private:
        /// Parse the given target path inside an archive in various
        /// stages
        void process(const std::vector<std::string> &dirs);
        void processVarIndex(const std::string &index);
        void processFrameIndex(const std::string &index);
        void processName(const std::string &name);

        std::string m_group;
        std::string m_name;
        std::string m_index;
        Behavior m_behavior;
        Format m_format;
        Resolution m_resolution;
    };

}

#ifdef GTAR_NAMESPACE_PARENT
}
#endif
#endif
