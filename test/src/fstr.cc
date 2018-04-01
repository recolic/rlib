#include <rlib/string/string.hpp>
#include <rlib/stdio.hpp>

using namespace rlib;
using namespace rlib::literals;

int main()
{
    println("fuck {} at {} a.m."_format("hust", 8));
    return 0;
}

