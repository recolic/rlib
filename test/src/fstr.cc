#include <rlib/string.hpp>
#include <rlib/stdio.hpp>

using namespace rlib;
using namespace rlib::literals;

int main()
{
    rlib::impl::format_string_meta<rlib::meta_array<'f','u','c','k'>, std::string, std::string>(std::string("fuck"),std::string("shit"));

    println("fuck {} at {} a.m."_format("hust", 8));
    auto s = "shit {}/{}."_rs .format("???", 1.234);
    println(s);
    println("\\{} will be replaced but \\\\{} will be preserved like {}."_format("you"));
    return 0;
}

