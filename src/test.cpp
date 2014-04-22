
#include <iostream>
#include <vector>
#include <string>
#include "Archive.hpp"

using namespace gtar;
using namespace std;

int main()
{
    {
        Archive arch("test.zip", gtar::Write);

        string foo("this is a test string blah blah blah");

        vector<char> vec;

        for(string::iterator i(foo.begin()); i != foo.end(); ++i)
            vec.push_back(*i);

        arch.writeVec(string("foo/test.txt"), vec);
    }

    {
        Archive arch("test.zip", gtar::Read);

        SharedArray<char> res(arch.read("foo/test.txt"));

        for(SharedArray<char>::iterator iter(res.begin()); iter != res.end(); ++iter)
            std::cout << *iter;
        std::cout << std::endl;
    }
    return 0;
}
