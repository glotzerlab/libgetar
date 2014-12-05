
#include "GTAR.hpp"

#include <cstdlib>
#include <iostream>
#include <string>

using namespace gtar;
using namespace std;

int main()
{
    int result(0);

    for(size_t i(0); i < 5; ++i)
    {
        GTAR arch("test.zip", Write);
        int randomUniform(rand());

        arch.writeUniform<int>("test.i64.uni", randomUniform);
        arch.close();

        GTAR readArch("test.zip", Read);
        auto_ptr<int> readUniform(readArch.readUniform<int>("test.i64.uni"));

        if(!readUniform.get())
        {
            cerr << "readUniform() returned a null pointer for a value "
                 << "which should be present" << endl;
            ++result;
        }
        else if(randomUniform != *readUniform)
        {
            cerr << "readUniform() returned a value which was not written" << endl;
            ++result;
        }

        if(readArch.readUniform<int>("non existent file").get())
        {
            cerr << "readUniform() on a file that doesn't exist returned a non-null pointer"
                 << endl;
            ++result;
        }

    }

    for(size_t i(0); i < 5; ++i)
    {
        GTAR arch("test.zip", Write);
        int randomIndividual[5];

        for(size_t i(0); i < sizeof(randomIndividual)/sizeof(int); ++i)
            randomIndividual[i] = rand();

        arch.writeIndividual<int*, int>("test.i64.uni", randomIndividual,
                                        randomIndividual + sizeof(randomIndividual),
                                        NoCompress);
        arch.close();

        GTAR readArch("test.zip", Read);
        SharedArray<int> readIndividual(readArch.readIndividual<int>("test.i64.uni"));

        if(!readIndividual.size())
        {
            cerr << "readIndividual() returned no result for a value "
                 << "which should be present" << endl;
            ++result;
        }
        else
        {
            bool equal(true);
            for(size_t i(0); i < sizeof(randomIndividual)/sizeof(int); ++i)
                equal |= randomIndividual[i] == readIndividual[i];

            if(!equal)
            {
                cerr << "readIndividual() returned a value which was not written" << endl;
                ++result;
            }
        }

        if(readArch.readIndividual<float>("non existent file").size())
        {
            cerr << "readIndividual() on a file that doesn't exist returned something"
                 << endl;
            ++result;
        }

    }

    return result;
}
