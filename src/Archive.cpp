// Archive.cpp
// by Matthew Spellings <mspells@umich.edu>

#include "Archive.hpp"

#ifdef GTAR_NAMESPACE_PARENT
namespace GTAR_NAMESPACE_PARENT{
#endif

namespace gtar{

    using std::string;
    using std::vector;

    Archive::~Archive() {}

    void Archive::writeVec(const string &path, const vector<char> &contents,
                           CompressMode mode, bool immediate)
    {
        writePtr(path, (void*) &contents[0], contents.size(), mode, immediate);
    }
}

#ifdef GTAR_NAMESPACE_PARENT
}
#endif
