#include <rlib/stdio.hpp>
#include <rlib/terminal.hpp>
using namespace rlib;
using namespace rlib::terminal;

int main() {
    auto cter = printfln<2>("{}Hello, {}={}, miao{}.{}", color_t::red, 6.6, 7, "www", clear);
    printfln("cter={}.", cter);
    println("test");
    return 0;
}
