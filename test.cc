#include <iostream>
#include "terminal.hpp"
using namespace std;
int main()
{
	cout << "\033[1m6666666\033[31m666666666\033[0m666666" << endl;
	cout << rlib::color_t::red << rlib::font_t::bold << "hello world" << rlib::fontInfo::clear() << "66666" << endl;
	return 0;
}
