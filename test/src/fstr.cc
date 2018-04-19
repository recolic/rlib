#include <rlib/string.hpp>
#include <rlib/stdio.hpp>

using namespace rlib;
using namespace rlib::literals;

int main()
{
    println("fuck {} at {} a.m."_format("hust", 8));
    auto s = "shit {}/{}."s .format("???", 1.234);
    println(s);
    return 0;
}

