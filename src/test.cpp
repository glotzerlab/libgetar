
#include <iostream>
#include <vector>
#include <string>
#include "Archive.hpp"
#include "GTAR.hpp"

using namespace gtar;
using namespace std;

int main()
{
    {
        GTAR arch("test.zip", gtar::Write);

        string foo("this is a test string blah blah blah");

        arch.writeString(string("foo/test.txt"), foo);
        arch.writeUniform<int>(string("test.uniform"), (int) 23);
    }

    {
        GTAR arch("test.zip", gtar::Read);

        SharedArray<char> res(arch.readBytes(string("foo/test.txt")));
        int res2(arch.readUniform<int>(string("test.uniform")));

        for(SharedArray<char>::iterator iter(res.begin()); iter != res.end(); ++iter)
            std::cout << *iter;
        std::cout << std::endl;
        std::cout << res2 << std::endl;
    }
    return 0;
}
