#ifndef R_STRING_HPP
#define R_STRING_HPP

#include <vector>
#include <string>
#include <sstream>
#include <type_traits>
//Must link libr.a
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
}

#endif