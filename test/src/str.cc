#include <rlib/string/string.hpp>
using namespace rlib;
using namespace rlib::literals;

#include <rlib/stdio.hpp>

void test(const string &s)
{
    println_iter(s.split());
    println_iter(s.split("w"));
    println(">>>", string("|").join(s.split()));
}

int main()
{
    test("dsaf wefew fwef we ");
    test("sfwaef wef wef wefew fwef eg") ;
    test("");
    test(">PAQ P<DSP<")   ;

    println(string(" 87sd6 8s7d5 8    8 75    ").strip() + "|");
    string s = "fuck             you                  r mom 34qwo0 ghwerf 0832 ";
    s.replace_inplace("ghw", "fuck you");
    println(s);
    s.replace_inplace("    ", "  ");
    println(s);
    
    println("{} are {} shits."_format("hust and hust", 2));
    println("?"rs .join("   shit !! ..."rs .split()));
    return 0;
}

