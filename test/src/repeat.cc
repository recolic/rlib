#include <rlib/functional.hpp>
#include <rlib/stdio.hpp>


int main() {
    auto f = rlib::repeat(4, [](int i){
            rlib::println("i is", i);
            }, 777);
    f();
}

