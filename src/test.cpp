
#include <vector>
#include <string>
#include "Archive.hpp"

using namespace gtar;
using namespace std;

int main()
{
    Archive arch("test.zip", gtar::Write);

    string foo("this is a test string blah blah blah");

    vector<char> vec;

    for(string::iterator i(foo.begin()); i != foo.end(); ++i)
        vec.push_back(*i);

    arch.writeVec(string("foo/test.txt"), vec);
    return 0;
}
