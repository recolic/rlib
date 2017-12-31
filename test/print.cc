#include <rlib/stdio.hpp>
#include <rlib/terminal.hpp>
using namespace rlib;

int main() {
    auto cter = printfln("{}Hello, {}={}, miao{}.{}", color_t::red, 6.6, 7, "www", clear);
    printfln("cter={}.", cter);
    return 0;

}
