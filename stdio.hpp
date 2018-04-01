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
#include <rlib/sys/fdset.hpp> // fd
#include <string>
#include <iostream>
#include <rlib/string/string.hpp> // format_string
#include <unistd.h> // STDOUT_FILENO

//TODO: update os.hpp for determine gcc/msvc easier
namespace rlib {
    namespace impl {
#if is_gcc
#include <ext/stdio_filebuf.h>
constexpr inline std::istream fd_to_istream(fd posix_handle) {
     __gnu_cxx::stdio_filebuf<char> filebuf(posix_handle, std::ios::in);
     return std::move(std::istream(filebuf));
}
#elif is_msvc
constexpr inline std::istream fd_to_istream(fd posix_handle) {
    ifstream ifs(::_fdopen(posix_handle, "r"));
    return std::move(ifs);
}
#else
constexpr inline std::istream fd_to_istream(fd handle) {
    if constexpr(handle == STDIN_FILENO)
        return std::cin;
    else
        throw std::invalid_argument("filefd != 1 to stream is not implemented except gcc/msvc.");
}
#endif
    } 
}

namespace rlib {
    template <size_t Fd = STDOUT_FILENO, typename PrintFinalT>
    void print(PrintFinalT reqArg);
    template <size_t Fd = STDOUT_FILENO, typename Required, typename... Optional>
    void print(Required reqArgs, Optional... optiArgs);
    template <size_t Fd = STDOUT_FILENO, typename... Optional>
    void println(Optional... optiArgs);
    template <size_t Fd = STDOUT_FILENO>
    void println();

    template <size_t Fd = STDOUT_FILENO, typename Iterable, typename Printable>
    void print_iter(Iterable arg, Printable spliter);
    template <size_t Fd = STDOUT_FILENO, typename Iterable, typename Printable>
    void println_iter(Iterable arg, Printable spliter);
    template <size_t Fd = STDOUT_FILENO, typename Iterable>
    void print_iter(Iterable arg);
    template <size_t Fd = STDOUT_FILENO, typename Iterable>
    void println_iter(Iterable arg);

    template <size_t Fd = STDOUT_FILENO, typename... Args>
    size_t printf(const std::string &fmt, Args... args);
    template <size_t Fd = STDOUT_FILENO, typename... Args>
    size_t printfln(const std::string &fmt, Args... args);

    template <size_t Fd = STDIN_FILENO>
    std::string scanln()
    {
        std::string line;
        std::getline(std::cin, line);
        return std::move(line);
    }

// Implements.
    extern bool enable_endl_flush;
    template < class CharT, class Traits >
    std::basic_ostream<CharT, Traits>& endl(std::basic_ostream<CharT, Traits>& os) {
        os << '\n';
        if(enable_endl_flush)
            os.flush();
        return os;
    }

    template <size_t Fd = STDOUT_FILENO, typename PrintFinalT>
    void print(PrintFinalT reqArg)
    {
        std::cout << reqArg;
    }
    template <size_t Fd = STDOUT_FILENO, typename Required, typename... Optional>
    void print(Required reqArgs, Optional... optiArgs)
    {
        std::cout << reqArgs << ' ';
        print(optiArgs ...);
    }
    template <size_t Fd = STDOUT_FILENO, typename... Optional>
    void println(Optional... optiArgs)
    {
        print(optiArgs ...);
        println();
    }
    template <size_t Fd = STDOUT_FILENO> 
    void println()
    {
        std::cout << rlib::endl;
    }

    template <size_t Fd = STDOUT_FILENO, typename Iterable, typename Printable>
    void print_iter(Iterable arg, Printable spliter)
    {
        for(const auto & i : arg)
            std::cout << i << spliter;
    }
    template <size_t Fd = STDOUT_FILENO, typename Iterable, typename Printable>
    void println_iter(Iterable arg, Printable spliter)
    {
        print_iter(arg, spliter);
        std::cout << rlib::endl;
    }
    template <size_t Fd = STDOUT_FILENO, typename Iterable>
    void print_iter(Iterable arg)
    {
        for(const auto & i : arg)
            std::cout << i << ' ';
    }
    template <size_t Fd = STDOUT_FILENO, typename Iterable>
    void println_iter(Iterable arg)
    {
        print_iter(arg);
        std::cout << rlib::endl;
    }

    template <size_t Fd = STDOUT_FILENO, typename... Args>
    size_t printf(const std::string &fmt, Args... args)
    {
        std::string to_print = format_string(fmt, args...); 
        std::cout << to_print;
        return to_print.size();
    }
    template <size_t Fd = STDOUT_FILENO, typename... Args>
    size_t printfln(const std::string &fmt, Args... args)
    {
        size_t len = rlib::printf(fmt, args...);
        std::cout << rlib::endl;
        return len + 1;
    }
}


#endif
