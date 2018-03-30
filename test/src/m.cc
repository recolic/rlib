#include <rlib/macro.hpp>

#define SELE_1 10001
#define SELE_0 10000

constexpr int mod(int n) {return n%2;}

#define GEN_M (mod(__COUNTER__))
#define S MACRO_CAT(SELE_, GEN_M)


#include <rlib/stdio.hpp>
using rlib::io;
int main()
{
    io::println(S);
    println(S);
    println(S);
    println(S);
    println(S);
    println(S);
    println(S);
    println(S);
    println(S);
    println(S);
    println(S);
    println(S);
    println(S);

    return 0;
}



