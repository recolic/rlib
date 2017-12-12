#ifndef R_STDIO_HPP
#define R_STDIO_HPP

#include <iostream>

namespace rlib {
    class io {
    public:
    	template<typename PrintFinalT>
    	static void print(PrintFinalT reqArg)
    	{
    		::std::cout << reqArg;
    		return;
    	}
    	template<typename Required, typename... Optional>
    	static void print(Required reqArgs, Optional... optiArgs)
    	{
    		::std::cout << reqArgs << ' ';
    		print(optiArgs ...);
    		return;
    	}
    	template<typename... Optional>
    	static void println(Optional... optiArgs)
    	{
    		print(optiArgs ...);
    		::std::cout << ::std::endl;
    		return;
    	}
    
    	template<typename Iterable, typename Printable>
    	static void print_iter(Iterable arg, Printable spliter)
    	{
            for(const auto & i : arg)
    		    ::std::cout << i << spliter;
    		return;
    	}
    	template<typename Iterable, typename Printable>
    	static void println_iter(Iterable arg, Printable spliter)
    	{
    		print_iter(arg, spliter);
    		::std::cout << ::std::endl;
    		return;
    	}
        template<typename Iterable>
    	static void print_iter(Iterable arg)
    	{
            for(const auto & i : arg)
    		    ::std::cout << i << ' ';
    		return;
    	}
    	template<typename Iterable>
    	static void println_iter(Iterable arg)
    	{
    		print_iter(arg);
    		::std::cout << ::std::endl;
    		return;
    	}
    
        static std::string scanln()
        {
            ::std::string str;
            ::std::getline(::std::cin, str);
            return std::move(str);
        }
    };
}

#endif
