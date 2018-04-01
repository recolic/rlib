#include <rlib/functional.hpp>
#include <rlib/stdio.hpp>


int main() {
//    auto f = rlib::repeat(4, [](int i){
//            rlib::println("i is", i);
//            }, 777);
//    f();
    auto m = [](int i){
        rlib::println("i is", i);
    };
    auto f = rlib::repeat(4, m, 777);
//    f(4,m,444);
//    auto ff = std::bind(&decltype(f)::operator(), &f, 4, m, 444);
    //std::function<void(size_t, decltype(m), int)> goodf(f);

//    auto ff = std::bind(f, 4,m,444);
    f();

}

