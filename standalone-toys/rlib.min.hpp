/*
 *
 * string.hpp: string process utility.
 * Recolic Keghart <root@recolic.net>
 * MIT License
 *
 * Minified version: works on C++11.
 *
 */

#ifndef R_STRING_HPP
#define R_STRING_HPP

#include <vector>
#include <string>
#include <stdexcept>

namespace rlib {
    class string : public std::string {
    public:
        using std::string::string;
        string() : std::string() {}
        string(const std::string &s) : std::string(s) {}
        string(std::string &&s) : std::string(std::forward<std::string>(s)) {}

    private:
        template <typename T> struct as_helper {};
        template <typename T>
        T as(as_helper<T>) const {
            if(empty()) return T();
            return T(*this);
        }
        const char *as(as_helper<const char *>) const {
            return this->c_str();
        }
        std::string as(as_helper<std::string>) const {
            return std::move(*this);
        }
        rlib::string as(as_helper<rlib::string>) const {
            return std::move(*this);
        }
        char as(as_helper<char>) const {
            if(size() > 1)
                throw std::invalid_argument("Can not convert rlib::string to char: size() > 1.");
            return size() == 0 ? '\0' : *cbegin();
        }
        // unsigned-char conflicts with uint8_t. I'll regard it as uint8_t. ("8".as<unsigned char> == 8)
        //unsigned char as(as_helper<unsigned char>) const {
        //    return static_cast<unsigned char>(as<char>());
        //}
        bool as(as_helper<bool>) const {
            if(*this == "true") {
                return true;
            }
            else if(*this == "false") {
                return false;
            }
            // Nothing is slower than throw(); Just test more cases...
            else if(*this == "1" || *this == "True" || *this == "TRUE") {
                return true;
            }
            else if(*this == "0" || *this == "False" || *this == "FALSE") {
                return false;
            }
            throw std::invalid_argument("Can not convert rlib::string to bool. Not matching any template.");
        }

#define RLIB_IMPL_GEN_AS_NUMERIC(type, std_conv) \
        type as(as_helper<type>) const { \
            if(empty()) return 0; \
            return std::std_conv(*this); \
        }

        RLIB_IMPL_GEN_AS_NUMERIC(int, stoi)
        RLIB_IMPL_GEN_AS_NUMERIC(long, stol)
        RLIB_IMPL_GEN_AS_NUMERIC(unsigned long, stoul)
        RLIB_IMPL_GEN_AS_NUMERIC(unsigned long long, stoull)
        RLIB_IMPL_GEN_AS_NUMERIC(long long, stoll)
        RLIB_IMPL_GEN_AS_NUMERIC(float, stof)
        RLIB_IMPL_GEN_AS_NUMERIC(double, stod)
        RLIB_IMPL_GEN_AS_NUMERIC(long double, stold)

#define RLIB_IMPL_GEN_AS_ALIAS(new_type, old_type) \
        new_type as(as_helper<new_type>) const { \
            return static_cast<new_type>(as<old_type>()); \
        }

        RLIB_IMPL_GEN_AS_ALIAS(unsigned int, unsigned long)
        RLIB_IMPL_GEN_AS_ALIAS(unsigned short, unsigned long)
        RLIB_IMPL_GEN_AS_ALIAS(uint8_t, unsigned long)

        RLIB_IMPL_GEN_AS_ALIAS(short, int)
        RLIB_IMPL_GEN_AS_ALIAS(int8_t, int)

    public:
        template <typename T>
        T as() const {
            return std::forward<T>(as(as_helper<T>()));
        }

        template <typename T>
        std::vector<T> split_as(const char &divider = ' ') const {
            const string &toSplit = *this;
            std::vector<T> buf;
            size_t curr = 0, prev = 0;
            while((curr = toSplit.find(divider, curr)) != std::string::npos) {
                buf.push_back(string(toSplit.substr(prev, curr - prev)).as<T>());
                ++curr; // skip divider
                prev = curr;
            }
            buf.push_back(string(toSplit.substr(prev)).as<T>());
            return std::move(buf);
        }
        template <typename T>
        std::vector<T> split_as(const std::string &divider) const {
            const string &toSplit = *this;
            std::vector<T> buf;
            size_t curr = 0, prev = 0;
            while((curr = toSplit.find(divider, curr)) != std::string::npos) {
                buf.push_back(string(toSplit.substr(prev, curr - prev)).as<T>());
                curr += divider.size(); // skip divider
                prev = curr;
            }
            buf.push_back(string(toSplit.substr(prev)).as<T>());
            return std::move(buf);
        }

        template <class ForwardIterable>
        string &join(const ForwardIterable &buffer) {
            join(buffer.cbegin(), buffer.cend());
            return *this;
        }
        template <class ForwardIterator>
        string &join(ForwardIterator begin, ForwardIterator end) {
            const string &toJoin = *this;
            std::string result;
            for(ForwardIterator iter = begin; iter != end; ++iter) {
                if(iter != begin)
                    result += toJoin;
                result += *iter;
            }
            return operator=(std::move(result));
        }

        string &strip() {
            strip(" \t\r\n");
            return *this;
        }
        template <typename CharOrStringOrView>
        string &strip(const CharOrStringOrView &stripped) {
            size_t len = size();
            size_t begin = find_first_not_of(stripped);

            if(begin == std::string::npos) {
                clear();
                return *this;
            }
            size_t end = find_last_not_of(stripped);

            erase(end + 1, len - end - 1);
            erase(0, begin);
            return *this;
        }

        string &replace(const std::string &from, const std::string &to) {
            size_t _;
            replace(from, to, _);
            return *this;
        }
        string &replace(const std::string &from, const std::string &to, size_t &out_times) {
            if(from.empty())
                return *this;
            size_t start_pos = 0;
            size_t times = 0;
            while((start_pos = find(from, start_pos)) != std::string::npos)
            {
                ++times;
                this->std::string::replace(start_pos, from.length(), to);
                start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
            }
            out_times = times;
            return *this;
        }
        string &replace_once(const std::string &from, const std::string &to) {
            bool _;
            replace_once(from, to, _);
            return *this;
        }
        string &replace_once(const std::string &from, const std::string &to, bool &out_replaced) {
            size_t start_pos = find(from);
            if(start_pos == std::string::npos) {
                out_replaced = false;
            }
            else {
                this->std::string::replace(start_pos, from.length(), to);
                out_replaced = true;
            }
            return *this;
        }


    };
}

#endif

#include <iostream>
#include <string>

namespace rlib {
    // This is my own hand-written library. I'm making it easy to use it directly.

    inline rlib::string scanln(std::istream &is = std::cin, char delimiter = '\n') noexcept {
        std::string line;
        std::getline(is, line, delimiter);
        return (line); // RVO
    }

    template <typename PrintFinalT>
    void print(PrintFinalT reqArg)
    {
        std::cout << reqArg;
    }
    template <typename Required, typename... Optional>
    void print(Required reqArgs, Optional... optiArgs)
    {
        std::cout << reqArgs << ' ';
        print(optiArgs ...);
    }
    template <typename... Optional>
    void println(Optional... optiArgs)
    {
        print(optiArgs ...);
        println();
    }
    template <> 
    inline void println()
    {
        //std::cout << rlib::endl;
        std::cout << std::endl;
    }
}

