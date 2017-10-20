#include <rlib/opt.hpp>

#include <cstdio>
#include <iostream>
#include <iomanip>
using std::cout;
using std::endl;
int main( int argl, char **argv)
{
    rlib::opt_parser opt(argl, argv);
    cout << std::boolalpha ;
    cout << opt.getValueArg("--fuck") << opt.getValueArg("--shit", "-s", true) << opt.getBoolArg("--boolt", "-b") << endl;
    return 0;
}

