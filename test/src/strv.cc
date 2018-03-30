#include <string_view>
using std::string_view;

//#include <rlib/require/cxx17>
#include <rlib/print.hpp>
using rlib::println;

int main(){
    string_view sv("test");
    println(sv);
    return 0;    


}
