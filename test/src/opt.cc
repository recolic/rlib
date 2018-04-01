#include <rlib/opt.hpp>
#include <rlib/stdio.hpp>
using rlib::println;
using rlib::print;

#include <iomanip>
#include <cassert>
int main(int argl, char **argv)
{
    rlib::opt_parser opt(argl, argv);
    print(std::boolalpha);
    println(opt.getValueArg("--fuck"), opt.getValueArg("--shit", "-s", true), opt.getBoolArg("--boolt", "-b"));
    println("ALldone:", opt.allArgDone());
    return 0;
}

