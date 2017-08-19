#ifndef R_IO_HPP
#define R_IO_HPP

#include <iostream>

namespace rlib {
	template<typename PrintFinalT>
	void print(PrintFinalT reqArg)
	{
		::std::cout << reqArg;
		return;
	}
	template<typename Required, typename... Optional>
	void print(Required reqArgs, Optional... optiArgs)
	{
		::std::cout << reqArgs << ' ';
		print(optiArgs ...);
		return;
	}
	template<typename... Optional>
	void println(Optional... optiArgs)
	{
		print(optiArgs ...);
		::std::cout << ::std::endl;
		return;
	}
}

#endif
