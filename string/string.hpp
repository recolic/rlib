#ifndef R_STRING_HPP
#define R_STRING_HPP

#include <vector>
#include <string>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <stdexcept>
#include <sstream>
#include <type_traits>

namespace rlib {
	std::vector<std::string> splitString(const std::string &tod, const char divider = ' ');
    size_t replaceSubString(std::string& str, const std::string &from, const std::string& to);
    bool replaceSubStringOnce(std::string& str, const std::string& from, const std::string& to);
    template<typename... Args>
    std::string format_string_c(const std::string &fmt, Args... args);
    template<typename... Args>
    std::string format_string(const std::string &fmt, Args... args);


//Implements.
    char *_format_string_c_helper(const char *fmt, ...);
    template<typename... Args>
    std::string format_string_c(const std::string &fmt, Args... args)
    {
        char *res = _format_string_c_helper(fmt.c_str(), args ...);
        std::string s = res;
        free(res);
        return std::move(s);
    }

    template<typename StdString>
    void _format_string_helper(std::stringstream &ss, const StdString &fmt) {
		static_assert(std::is_same<StdString, std::string>::value, "incorrect argument type to _format_string_helper");
        ss << fmt;
    }
    template<typename Arg1, typename... Args>
    void _format_string_helper(std::stringstream &ss, const std::string &fmt, Arg1 arg1, Args... args) {
        size_t pos = 0;
        while((pos = fmt.find("{}")) != std::string::npos) {
            if(pos != 0 && fmt[pos-1] == '\\') {
                ++pos;
                continue;
            }
            ss << fmt.substr(0, pos) << arg1;
            _format_string_helper(ss, fmt.substr(pos + 2), args ...);
            return;
        }
		_format_string_helper(ss, fmt);
    }
    template<typename... Args>
    std::string format_string(const std::string &fmt, Args... args) {
        std::stringstream ss;
        _format_string_helper(ss, fmt, args...);
        return ss.str();
    }

	inline std::vector<std::string> splitString(const std::string &tod, const char divider)
	{
		size_t lastPos = 0;
		size_t thisPos = tod.find(divider);
	    std::vector<std::string> sbuf;
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
    inline size_t replaceSubString(std::string& str, const std::string &from, const std::string& to) 
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
    inline bool replaceSubStringOnce(std::string& str, const std::string& from, const std::string& to) 
    {
        size_t start_pos = str.find(from);
        if(start_pos == std::string::npos)
            return false;
        str.replace(start_pos, from.length(), to);
        return true;
    }

    inline char *_format_string_c_helper(const char *fmt, ...)
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