#include <rlib/string.hpp>
#include <rlib/stdio.hpp>

using namespace rlib;
using namespace rlib::literals;

int main()
{
    println("fuck {} at {} a.m."_format("hust", 8));
    auto s = "shit {}/{}."_rs .format("???", 1.234);
    println(s);
    println("\\{} will be replaced but \\\\{} will be preserved like {}."_format("you"));
    return 0;
}

