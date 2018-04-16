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
#include <cstring>
#include <stdexcept>
#include <sstream>
#include <type_traits>

namespace rlib {
    // literals::_format, format_string, string::format
    namespace impl {
        template<typename StdString>
        void _format_string_helper(std::stringstream &ss, const StdString &fmt) {
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

    // format_string_c, string::cformat
    namespace impl {
        inline char *_format_string_c_helper(const char *fmt, ...)
        {
            int n;
            int size = std::strlen(fmt);     /* Guess we need no more than 100 bytes */
            char *p, *np;
            va_list ap;

            if ((p = (char *)malloc(size)) == NULL)
                throw std::runtime_error("malloc returns null.");

            while (1) {
                va_start(ap, fmt);
                n = vsnprintf(p, size, fmt, ap);
                va_end(ap);

                if (n < 0)
                    throw std::runtime_error("vsnprintf returns " + std::to_string(n));
                if (n < size)
                    return p;

                size = n + 1;

                if ((np = (char *)realloc (p, size)) == NULL) {
                    free(p);
                    throw std::runtime_error("make_message realloc failed.");
                } else {
                    p = np;
                }
            }
        }
        template<typename... Args>
        std::string format_string_c(const std::string &fmt, Args... args)
        {
            char *res = _format_string_c_helper(fmt.c_str(), args ...);
            std::string s = res;
            free(res);
            return std::move(s);
        }
    }

    class string : public std::string {
    public:
        using std::string::string;
        // Warning: Allocator not supported. Some unusual constructed not supported.
        string(const std::string &s) : std::string(s) {}
        string(std::string &&s) : std::string(std::forward<std::string>(s)) {}
        //string(const char *s, size_t count) : std::string(s, count) {}
        //string(const char *s) : std::string(s) {}
        //string(size_t count, char c) : std::string(count, c) {}
        //string(const std::string &other, size_t pos) : std::string(other, pos) {}
        //string(const std::string &other, size_t pos, size_t count) : std::string(other, pos, count) {}
        //template<typename InputIt> string(InputIt first, InputIt last) : std::string(first, last) {}        

        std::vector<string> split(const char &divider = ' ') const {
            const string &toSplit = *this;
            std::vector<string> buf;
            size_t curr = 0, prev = 0;
            while((curr = toSplit.find(divider, curr)) != std::string::npos) {
                buf.push_back(toSplit.substr(prev, curr - prev));
                ++curr; // skip divider
                prev = curr;
            }
            buf.push_back(toSplit.substr(prev));
            return std::move(buf);
        }
        std::vector<string> split(const std::string &divider) const {
            const string &toSplit = *this;
            std::vector<string> buf;
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
        string join(ForwardIterator begin, ForwardIterator end) const {
            const string &toJoin = *this;
            std::string result;
            for(ForwardIterator iter = begin; iter != end; ++iter) {
                if(iter != begin)
                    result += toJoin;
                result += *iter;
            }
            return std::move(result);
        }
        template <class ForwardIterable>
        string join(const ForwardIterable &buffer) const {
            return std::move(this->join(buffer.cbegin(), buffer.cend()));
        }

        template <typename ForwardIterator>
        string strip(ForwardIterator begin, ForwardIterator end) const;
        template <class ForwardIterable>
        string strip(const ForwardIterable &strippedCharBuffer) const;
        string strip(char stripped = ' ') const {
            const string &toStrip = *this;
            auto len = toStrip.size();
            size_t begin = 0;
            size_t end = len - 1;
            while(toStrip[begin] == stripped) ++begin;
            while(end != begin && toStrip[end] == stripped) --end;
            return std::move(toStrip.substr(begin, end+1));
        }

        //TODO: Change name so it won't fight with std::string
        size_t replace_inplace(const std::string &from, const std::string &to) {
            if(from.empty())
                return 0;
            size_t start_pos = 0;
            size_t times = 0;
            while((start_pos = find(from, start_pos)) != std::string::npos)
            {
                ++times;
                replace(start_pos, from.length(), to);
                start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
            }
            return times;
        }
        bool replace_once_inplace(const std::string &from, const std::string &to) {
            size_t start_pos = find(from);
            if(start_pos == std::string::npos)
                return false;
            replace(start_pos, from.length(), to);
            return true;
        }

        template <typename... Args>
        string format(Args... args) const {
            return std::move(impl::format_string(*this, args ...));
        }
        template <typename... Args>
        string cformat(Args... args) const {
            return std::move(impl::format_string_c(*this, args ...));
        }
    };

    namespace impl {
        struct formatter {
            formatter(const std::string &fmt) : fmt(fmt) {}
            formatter(std::string &&fmt) : fmt(fmt) {}
            template <typename... Args>
            std::string operator ()(Args... args) {
                return std::move(rlib::impl::format_string(fmt, args ...));
            }

            std::string fmt;
        };
    }

    namespace literals {
        impl::formatter operator "" _format (const char *str, size_t) {
            return std::move(impl::formatter(str));
        }
        rlib::string operator "" s (const char *str, size_t len) {
            return std::move(rlib::string(str, len));
        }
    }


}

#endif
