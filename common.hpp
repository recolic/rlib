#ifndef R_COMMON_HPP
#define R_COMMON_HPP
#include <iterator>
#include "rstdafx.hpp"
_recolic_hpp_begin
template<typename _tData>
void CheckedDelete(_tData *p)
{
	if (p)
		delete p;
}
template<typename _tData>
void CheckedDeleteArr(_tData *p)
{
	if (p)
		delete[] p;
}

_recolic_hpp_end

#endif