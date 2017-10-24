#include <rlib/opt.hpp>
#include <rlib/print.hpp>
using rlib::println;
using rlib::print;

#include <iostream>
#include <iomanip>
using std::cout;
using std::endl;
int main( int argl, char **argv)
{
    rlib::opt_parser opt(argl, argv);
    print(std::boolalpha);
    println(opt.getValueArg("--fuck"), opt.getValueArg("--shit", "-s", true), opt.getBoolArg("--boolt", "-b"));
    println("ALldone:", opt.allArgDone());
    return 0;
}

