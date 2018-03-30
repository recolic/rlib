#include "rlib/string/fstr.hpp"
#include <iostream>
#include <string>


using namespace rlib;
using namespace std;

int main()
{
    string s = fstr("here:%s", "miaow");
    cout << s << endl;
    return 0;
}

