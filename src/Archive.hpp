// Archive.h
// by Matthew Spellings <mspells@umich.edu>

#include <memory>
#include <vector>
#include <string>

#include <archive.h>

namespace gtar{

    using std::auto_ptr;
    using std::string;
    using std::vector;

    enum OpenMode {Read, Write};

    class Archive
    {
    public:
        Archive(const string &filename, const OpenMode mode);

        ~Archive();

        void writeVec(const string &path, const vector<char> &contents);

        void writePtr(const string &path, const void *contents, const size_t byteLength);
    private:
        const string m_filename;
        const OpenMode m_mode;
        auto_ptr<archive> m_archiveptr;
    };

}
