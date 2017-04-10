#ifndef R_IO_HPP
#define R_IO_HPP
#include <iostream>
#include "rstdafx.hpp"
_recolic_hpp_begin
template<typename PrintFinalT>
void print(PrintFinalT reqArg)
{
	::std::cout << reqArg << ::std::endl;
	return;
}
template<typename Required, typename... Optional>
void print(Required reqArgs, Optional... optiArgs)
{
	::std::cout << reqArgs << ' ';
	print(optiArgs ...);
	return;
}
_recolic_hpp_end

#endif