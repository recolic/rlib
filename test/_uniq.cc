#include <rlib/print.hpp>
using rlib::println;

#include <stdexcept>
using std::runtime_error;

#include <functional>
#include <memory>

class c{
public:
    c() {println("cons");}
    ~c() {println("des");}
};


struct D { // deleter
    D() {};
    D(const D&) { std::cout << "D copy ctor\n"; }
    D(D&) { std::cout << "D non-const copy ctor\n";}
    D(D&&) { std::cout << "D move ctor \n"; }
    void operator()(int * p) const {
        std::cout << "D is deleting a Foo\n";
        delete p;
    };
};
 

int main()
{
    try
    {
        c cl;
        std::unique_ptr<int, D > p(new int(), D());
        throw runtime_error("fuck");
    }
    catch(std::exception &)
    {}
    return 0;
}