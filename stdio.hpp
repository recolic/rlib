#ifndef R_STDIO_HPP
#define R_STDIO_HPP

#include <iostream>

namespace rlib {
    class io {
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
    
    	template<typename Iterable, typename Printable>
    	void print_iter(Iterable arg, Printable spliter)
    	{
            for(const auto & i : arg)
    		    ::std::cout << i << spliter;
    		return;
    	}
    	template<typename Iterable, typename Printable>
    	void println_iter(Iterable arg, Printable spliter)
    	{
    		print_iter(arg, spliter);
    		::std::cout << ::std::endl;
    		return;
    	}
        template<typename Iterable>
    	void print_iter(Iterable arg)
    	{
            for(const auto & i : arg)
    		    ::std::cout << i << ' ';
    		return;
    	}
    	template<typename Iterable>
    	void println_iter(Iterable arg)
    	{
    		print_iter(arg);
    		::std::cout << ::std::endl;
    		return;
    	}
    
        std::string scanln()
        {
            ::std::string str;
            ::std::getline(::std::cin, str);
            return std::move(str);
        }
    };
}

#endif
