/*
 *
 * stdio wrapper for modern c++: python like print/println/printf/printfln
 *                               print_iter println_iter
 * Recolic Keghart <root@recolic.net>
 * MIT License
 *
 */

#ifndef R_STDIO_HPP
#define R_STDIO_HPP

#include <rlib/require/cxx11> // Use fold expression if cxx17 is available.
#include <rlib/sys/os.hpp> // Enable inline variable if cxx17 is available.
#include <string>
#include <iostream>
#include <rlib/string.hpp> // format_string
#include <type_traits>

#if RLIB_OS_ID == OS_WINDOWS
#define RLIB_IMPL_ENDLINE "\r\n"
#elif RLIB_OS_ID == OS_MACOS
#define RLIB_IMPL_ENDLINE "\r"
#else
#define RLIB_IMPL_ENDLINE "\n"
#endif

namespace rlib {
    namespace impl {
        template <typename T>
        struct print_wrapper {
            print_wrapper() = delete;
            print_wrapper(const T &dat)
                : wrapper(dat) {}

            const T &wrapper;

            friend std::ostream & operator<< (std::ostream &os, print_wrapper<T> p) {
                return os << p.wrapper;
            }
        };
    }
}

namespace rlib {
// print to custom stream
    template <typename PrintFinalT>
    void print(std::ostream &os, PrintFinalT reqArg);
    template <typename Required, typename... Optional>
    void print(std::ostream &os, Required reqArgs, Optional... optiArgs);
    template <typename... Optional>
    void println(std::ostream &os, Optional... optiArgs);
    template <>
    void println(std::ostream &os);

    template <typename Iterable, typename Printable>
    void print_iter(std::ostream &os, Iterable arg, Printable spliter);
    template <typename Iterable, typename Printable>
    void println_iter(std::ostream &os, Iterable arg, Printable spliter);
    template <typename Iterable>
    void print_iter(std::ostream &os, Iterable arg);
    template <typename Iterable>
    void println_iter(std::ostream &os, Iterable arg);

    template <typename... Args>
    size_t printf(std::ostream &os, const std::string &fmt, Args... args);
    template <typename... Args>
    size_t printfln(std::ostream &os, const std::string &fmt, Args... args);

    template <typename TargetType = std::string>
    inline TargetType scan(std::istream &is = std::cin) {
        TargetType target;
        is >> target;
        return target;
    }
    inline rlib::string scanln(std::istream &is = std::cin, char delimiter = '\n') noexcept {
        std::string line;
        std::getline(is, line, delimiter);
        return std::move(line);
    }

    // default for std::cout
    template <typename... Args>
    void println(Args... args);
    template <>
    void println();
    template <typename... Args>
    void print(Args... args);
    template <typename... Args>
    size_t printf(const std::string &fmt, Args... args);
    template <typename... Args>
    size_t printfln(const std::string &fmt, Args... args);

    // implementations below --------------------------------

    namespace impl {
        inline bool &enable_endl_flush() {
            static bool instance = true;
            return instance;
        }

        template <typename Iterable, typename Printable>
        struct _printable_iterable : private std::pair<Iterable, Printable> {
            using std::pair<Iterable, Printable>::pair;
            using _printable_iterable_tag = void;
            Iterable &arg() & {return std::pair<Iterable, Printable>::first;}
            Printable &spliter() & {return std::pair<Iterable, Printable>::second;}
            Iterable &&arg() && {return std::pair<Iterable, Printable>::first;}
            Printable &&spliter() && {return std::pair<Iterable, Printable>::second;}
            const Iterable &arg() const & {return std::pair<Iterable, Printable>::first;}
            const Printable &spliter() const & {return std::pair<Iterable, Printable>::second;}
        };
        template <typename FirstT, typename SecondT> using FirstOf = FirstT;
    }

    // more interfaces...
    template <typename Iterable, typename Printable = char>
    auto printable_iter(Iterable &&arg, Printable spliter = ' ') -> impl::_printable_iterable<typename std::decay<Iterable>::type, Printable> {
        // TODO: avoid the extra copy while passing lvalue reference on constructing return value obj. 
        return impl::_printable_iterable<typename std::decay<Iterable>::type, Printable>(std::forward<Iterable>(arg), spliter);
    }

    inline bool sync_with_stdio(bool sync = true) noexcept {
        return std::ios::sync_with_stdio(sync);
    }
    inline bool enable_endl_flush(bool enable = true) noexcept {
        return impl::enable_endl_flush() = enable;
    }

    // Implements below ---------------------
    template <typename CharT, typename Traits>
    inline std::basic_ostream<CharT, Traits>& endl(std::basic_ostream<CharT, Traits>& os) {
        os << RLIB_IMPL_ENDLINE;
        if(impl::enable_endl_flush())
            os.flush();
        return os;
    }

    // With custom os
    template <typename PrintFinalT>
    void print(std::ostream &os, PrintFinalT reqArg)
    {
        os << std::forward<PrintFinalT>(reqArg);
    }
    template <typename Required, typename... Optional>
    void print(std::ostream &os, Required reqArgs, Optional... optiArgs)
    {
        os << reqArgs << ' ';
        print(os, std::forward<Optional>(optiArgs) ...);
    }
    template <typename... Optional>
    void println(std::ostream &os, Optional... optiArgs)
    {
        print(os, std::forward<Optional>(optiArgs) ...);
        println(os);
    }
    template <> 
    inline void println(std::ostream &os)
    {
        os << rlib::endl;
    }

    template <typename... Args>
    size_t printf(std::ostream &os, const std::string &fmt, Args... args)
    {
        std::string to_print = impl::format_string(fmt, args...); 
        print(os, to_print);
        return to_print.size();
    }
    template <typename... Args>
    size_t printfln(std::ostream &os, const std::string &fmt, Args... args)
    {
        size_t len = printf(os, fmt, args...);
        println(os);
        return len + 1;
    }



    // default for std::cout
    template <typename... Args>
    void println(Args... args) {
        return println(std::cout, std::forward<Args>(args) ...);
    }
    template <>
    inline void println() {
        return println(std::cout);
    }
    template <typename... Args>
    void print(Args... args) {
        return print(std::cout, std::forward<Args>(args) ...);
    }
    template <typename... Args>
    size_t printf(const std::string &fmt, Args... args) {
        return printf(std::cout, fmt, std::forward<Args>(args) ...);
    }
    template <typename... Args>
    size_t printfln(const std::string &fmt, Args... args) {
        return printfln(std::cout, fmt, std::forward<Args>(args) ...);
    }


    // If the stream is stringstream or ostringstream,
    //   it will fails to match print(ostream &, args...),
    //   and match print(args ...). It leads to an error.
    // Here's the fallback on such sucking substitution error.
    template <typename StreamType, typename... Args>
    void println(StreamType &os, Args... args) {
        using ostream_or_data = typename std::conditional<std::is_base_of<std::ostream, StreamType>::value, 
            std::ostream &, impl::print_wrapper<StreamType>>::type;
        return println(static_cast<ostream_or_data>(os), std::forward<Args>(args) ...);
    }
    template <typename StreamType, typename... Args>
    void print(StreamType &os, Args... args) {
        using ostream_or_data = typename std::conditional<std::is_base_of<std::ostream, StreamType>::value, 
            std::ostream &, impl::print_wrapper<StreamType>>::type;
        return print(static_cast<ostream_or_data>(os), std::forward<Args>(args) ...);
    }
} // end namespace rlib

// auto-deduct const/nonconst left/right value ref. 
// For C++20 std::view, it doesn't have a `begin()` method for const lvalue ref. So we have to support 
//   passing rvalue from rlib::printable_iter() to rlib::impl::_printable_iterable to operator<< 
// Instead of writing 3 overloads here, let us deduce automatically. 
template <typename PrintableIterableT>
rlib::impl::FirstOf<std::ostream&, typename std::decay<PrintableIterableT>::type::_printable_iterable_tag> operator<< (std::ostream& stream, PrintableIterableT &&p) {
     for(auto val : p.arg())
         stream << val << p.spliter();
     return stream;
}
// // Old version, for backup
// template <typename Iterable, typename Printable>
// std::ostream& operator<< (std::ostream& stream, rlib::impl::_printable_iterable<Iterable, Printable> &&p) {
//     for(auto val : p.arg())
//         stream << val << p.spliter();
//     return stream;
// }

#endif
