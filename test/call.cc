#include <rlib/traits.hpp>

using namespace rlib;

void f(int);

class c{
public:
    auto operator()(int a){
        return a+1;
    }
};

int main(){
    static_assert(is_callable<decltype(f)>(), "a");
    static_assert(is_callable<c>(), "b");
}
