#include <rlib/functional.hpp>
#include <rlib/stdio.hpp>
using namespace rlib;

#define test_str "This is some test string."
#define test_times 1000000

int main() {
    println(time_of(repeat(test_times, []{
                    println(test_str);
                    })));
    println(time_of(repeat(test_times, []{
                    std::cout << test_str << std::endl;
                    })));
    println(time_of(repeat(test_times, []{
                    std::printf(test_str);
                    std::printf("\n");
                    })));
}
