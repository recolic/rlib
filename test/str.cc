#include <rlib/string/string.hpp>
using namespace rlib;

#include <rlib/stdio.hpp>
#include <string>
using namespace std;

void test(const string &s)
{
    println_iter(splitString(s));
    println_iter(splitString(s, " w"s));
    println(">>>", joinString("|", splitString(s)));
}

int main()
{
    test("dsaf wefew fwef we ");
    test("sfwaef wef wef wefew fwef eg") ;
    test("");
    test(">PAQ P<DSP<")   ;

    string s = "fuck             you                  r mom 34qwo0 ghwerf 0832 ";
    replaceSubString(s, "ghw", "fuck you");
    println(s);
    replaceSubString(s, "   ", "  ");
    println(s);
    return 0;
}

