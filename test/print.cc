#include <rlib/stdio.hpp>
using rlib::printfln;

int main() {
    auto cter = printfln("Hello, {}={}, miao{}.", 6.6, 7, "www");
    printfln("cter={}.", cter);
    return 0;

}