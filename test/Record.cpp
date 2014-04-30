
#include "Record.hpp"

#include <iostream>
#include <string>

using namespace gtar;
using namespace std;

// Get rid of any number of initial slashes ('/') in a string
string skipAbsolute(const string &str)
{
    size_t start(str.find_first_not_of('/'));
    return str.substr(start);
}

bool checkstr(const string &one, const string &two, bool shouldEqual, int line)
{
    string newOne(skipAbsolute(one));
    string newTwo(skipAbsolute(two));

    bool failure(shouldEqual ^ (newOne == newTwo));

    if(failure)
        cerr << "Error in line " << line << ": \"" << one << "\" != \"" << two << "\"" << endl;

    return failure;
}

bool check(const Record &rec, const string group, const string name,
           const string index, const string suffix, Behavior behavior,
           Format format, Resolution resolution, bool shouldEqual, int line)
{
    const Record ref(group, name, index, suffix, behavior, format, resolution);
    bool failure(shouldEqual ^ (ref == rec));

    if(failure)
        cerr << "Error in line " << line << endl;

    return failure;
}

int main()
{
    int result(0);

    {
        string str("frames/100/log.txt");
        Record rec(str);
        result |= check(rec, "", "log.txt", "100", "", Discrete, UInt8, Text, true, __LINE__);
        result |= checkstr(str, rec.getPath(), true, __LINE__);
    }

    {
        string str("frames/100/log.txt");
        Record rec(str);
        result |= check(rec, "", "lo.txt", "100", "", Discrete, UInt8, Text, false, __LINE__);
        result |= checkstr(str, rec.getPath(), true, __LINE__);
    }

    {
        string str("frames/100/position.f32.ind");
        Record rec(str);
        result |= check(rec, "", "position", "100", "", Discrete, Float32, Individual, true, __LINE__);
        result |= checkstr(str, rec.getPath(), true, __LINE__);
    }

    {
        string str("velocity.u64.uni");
        Record rec(str);
        result |= check(rec, "", "velocity", "", "", Constant, UInt64, Uniform, true, __LINE__);
        result |= checkstr(str, rec.getPath(), true, __LINE__);
    }

    {
        string str("/vars/stdout/12.txt");
        Record rec(str);
        result |= check(rec, "", "stdout", "12", ".txt", Continuous, UInt8, Text, true, __LINE__);
        result |= checkstr(str, rec.getPath(), true, __LINE__);
    }

    {
        string str("/rigid_body/frames/0/angular_momentum.f64.ind");
        Record rec(str);
        result |= check(rec, "rigid_body", "angular_momentum", "0", "", Discrete, Float64, Individual, true, __LINE__);
        result |= checkstr(str, rec.getPath(), true, __LINE__);
    }

    return result;
}
