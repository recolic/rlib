// Library implementations here, if necessary.

#if __cplusplus <= 199711L
#   error This library needs at least a C++11 compliant compiler
#endif

// C++11 parts here.
#if __cplusplus > 199711L

#include <iostream>
#include <vector>
#include <string>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <stdexcept>
#include <sstream>

//stdio
#include <rlib/stdio.hpp>

namespace rlib {
    bool enable_endl_flush = true;
    
    std::string scanln()
    {
        ::std::string line;
        ::std::getline(::std::cin, line);
        return std::move(line);
    }
}

// string/string.hpp
#include <rlib/string/string.hpp>

namespace rlib {
	using std::string;
	using std::vector;
	vector<string> splitString(const string &tod, const char divider)
	{
		size_t lastPos = 0;
		size_t thisPos = tod.find(divider);
	    vector<string> sbuf;
		if (thisPos != ::std::string::npos)
		{
			sbuf.push_back(tod.substr(0, thisPos));
			goto gt_1;
		}
		else
	    {
	        sbuf.push_back(tod);
	        return sbuf;
	    }
		do {
			sbuf.push_back(tod.substr(lastPos + 1, thisPos - lastPos - 1));
		gt_1:
			lastPos = thisPos;
			thisPos = tod.find(divider, lastPos + 1);
		} while (thisPos != ::std::string::npos);
		sbuf.push_back(tod.substr(lastPos + 1));
		return ::std::move(sbuf);
	}
    size_t replaceSubString(std::string& str, const std::string &from, const std::string& to) 
    {
        if(from.empty())
            return 0;
        size_t start_pos = 0;
        size_t times = 0;
        while((start_pos = str.find(from, start_pos)) != std::string::npos)
        {
            ++times;
            str.replace(start_pos, from.length(), to);
            start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
        }
        return times;
    }
    bool replaceSubStringOnce(std::string& str, const std::string& from, const std::string& to) 
    {
        size_t start_pos = str.find(from);
        if(start_pos == std::string::npos)
            return false;
        str.replace(start_pos, from.length(), to);
        return true;
    }

    char *_format_string_c_helper(const char *fmt, ...)
    {
        int n;
        int size = 100;     /* Guess we need no more than 100 bytes */
        char *p, *np;
        va_list ap;

        if ((p = (char *)malloc(size)) == NULL)
            throw std::runtime_error("malloc returns null.");

        while (1) {

            /* Try to print in the allocated space */

            va_start(ap, fmt);
            n = vsnprintf(p, size, fmt, ap);
            va_end(ap);

            /* Check error code */

            if (n < 0)
                throw std::runtime_error("vsnprintf returns " + std::to_string(n));

            /* If that worked, return the string */

            if (n < size)
                return p;

            /* Else try again with more space */

            size = n + 1;       /* Precisely what is needed */

            if ((np = (char *)realloc (p, size)) == NULL) {
                free(p);
                throw std::runtime_error("make_message realloc failed.");
            } else {
                p = np;
            }
        }
    }

}


#endif

// C++14 parts here.
#if __cplusplus > 201103L

#endif

// C++17 parts here.
#if __cplusplus > 201402L

#endif