/*
 *
 * string.hpp: string process utility.
 * Recolic Keghart <root@recolic.net>
 * MIT License
 *
 */

#ifndef R_STRING_HPP
#define R_STRING_HPP

#include <rlib/require/cxx14>
#include <rlib/class_decorator.hpp>

#include <vector>
#include <string>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <stdexcept>
#include <sstream>
#include <type_traits>

namespace rlib {
//TODO: Pass args by rvalue reference.

    class string : public std::string {
        std::vector<string> split(const char &divider = ' ');
        std::vector<string> split(const std::string &divider);

        template <class ForwardIterator>
        string join(ForwardIterator begin, ForwardIterator end);
        template <class ForwardIterable>
        string join(const ForwardIterable &buffer);

        template <typename ForwardIterator>
        string strip(ForwardIterator begin, ForwardIterator end);
        template <class ForwardIterable>
        string strip(const ForwardIterable &strippedCharBuffer);
        template <>
        string strip(char stripped = ' ');

        size_t replace(const std::string &from, const std::string &to);
        bool replace_once(const std::string &from, const std::string &to);

        template <typename... Args>
        string format(Args... args);
    };
	std::vector<std::string> splitString(const std::string &toSplit, const char &divider = ' ');
	std::vector<std::string> splitString(const std::string &toSplit, const std::string &divider);
    template <class ForwardIterator>
    std::string joinString(const char &toJoin, ForwardIterator begin, ForwardIterator end);
    template <class ForwardIterator>
    std::string joinString(const std::string &toJoin, ForwardIterator begin, ForwardIterator end);
    template <class ForwardIterable>
    std::string joinString(const char &toJoin, ForwardIterable begin, ForwardIterable end);
    template <class ForwardIterable>
    std::string joinString(const std::string &toJoin, ForwardIterable begin, ForwardIterable end);

    std::string stripString(const std::string &toStrip, char stripped = ' ');
  
    size_t replaceSubString(std::string& str, const std::string &from, const std::string& to);
    bool replaceSubStringOnce(std::string& str, const std::string& from, const std::string& to);
    template <typename... Args>
    std::string format_string_c(const std::string &fmt, Args... args);
    template <typename... Args>
    std::string format_string(const std::string &fmt, Args... args);

    namespace impl {
        struct formatter {
            formatter(const std::string &fmt) : fmt(fmt) {}
            formatter(std::string &&fmt) : fmt(fmt) {}
            template <typename... Args>
            std::string operator ()(Args... args) {
                return rlib::format_string(fmt, args ...);
            }

            std::string fmt;
        };
    }

    namespace literals {
        impl::formatter operator "" _format (const char *str, size_t) {
            return std::move(impl::formatter(str));
        }
    }

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

	inline std::vector<std::string> splitString(const std::string &toSplit, const char &divider)
	{
        std::vector<std::string> buf;
        size_t curr = 0, prev = 0;
        while((curr = toSplit.find(divider, curr)) != std::string::npos) {
            buf.push_back(toSplit.substr(prev, curr - prev));
            ++curr; // skip divider
            prev = curr;
        }
        buf.push_back(toSplit.substr(prev));
        return std::move(buf);
	}
    inline std::vector<std::string> splitString(const std::string &toSplit, const std::string &divider)
	{
        std::vector<std::string> buf;
        size_t curr = 0, prev = 0;
        while((curr = toSplit.find(divider, curr)) != std::string::npos) {
            buf.push_back(toSplit.substr(prev, curr - prev));
            curr += divider.size(); // skip divider
            prev = curr;
        }
        buf.push_back(toSplit.substr(prev));
        return std::move(buf);
	}
    template <class ForwardIterator>
    std::string joinString(const char &toJoin, ForwardIterator begin, ForwardIterator end) {
        std::string result;
        for(ForwardIterator iter = begin; iter != end; ++iter) {
            if(iter != begin)
                result += toJoin;
            result += *iter;
        }
        return std::move(result);
    }
    template <class ForwardIterator>
    std::string joinString(const std::string &toJoin, ForwardIterator begin, ForwardIterator end) {
        std::string result;
        for(ForwardIterator iter = begin; iter != end; ++iter) {
            if(iter != begin)
                result += toJoin;
            result += *iter;
        }
        return std::move(result);
    }
    template <class ForwardIterable>
    std::string joinString(const std::string &toJoin, ForwardIterable buf) {
        auto begin = buf.begin();
        auto end = buf.end();
        return std::move(joinString(toJoin, begin, end));
    }
    template <class ForwardIterable>
    std::string joinString(const char &toJoin, ForwardIterable buf) {
        auto begin = buf.begin();
        auto end = buf.end();
        return std::move(joinString(toJoin, begin, end));
    }

    inline std::string stripString(const std::string &toStrip, char stripped) {
        auto len = toStrip.size();
        size_t begin = 0;
        size_t end = len - 1;
        while(toStrip[begin] == stripped) ++begin;
        while(end != begin && toStrip[end] == stripped) --end;
        return std::move(toStrip.substr(begin, end+1));
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
