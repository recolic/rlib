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
#include <rlib/sys/fd.hpp> // fd
#include <string>
#include <iostream>
#include <rlib/string/string.hpp> // format_string
#include <unistd.h> // STDOUT_FILENO

namespace rlib {
    template <fd Fd = STDOUT_FILENO, typename PrintFinalT>
    void print(PrintFinalT reqArg);
    template <fd Fd = STDOUT_FILENO, typename Required, typename... Optional>
    void print(Required reqArgs, Optional... optiArgs);
    template <fd Fd = STDOUT_FILENO, typename... Optional>
    void println(Optional... optiArgs);
    template <fd Fd = STDOUT_FILENO>
    void println();

    template <fd Fd = STDOUT_FILENO, typename Iterable, typename Printable>
    void print_iter(Iterable arg, Printable spliter);
    template <fd Fd = STDOUT_FILENO, typename Iterable, typename Printable>
    void println_iter(Iterable arg, Printable spliter);
    template <fd Fd = STDOUT_FILENO, typename Iterable>
    void print_iter(Iterable arg);
    template <fd Fd = STDOUT_FILENO, typename Iterable>
    void println_iter(Iterable arg);

    template <fd Fd = STDOUT_FILENO, typename... Args>
    size_t printf(const std::string &fmt, Args... args);
    template <fd Fd = STDOUT_FILENO, typename... Args>
    size_t printfln(const std::string &fmt, Args... args);

    template <fd Fd = STDIN_FILENO>
    std::string scanln()
    {
        std::string line;
        std::getline(fd_to_istream<Fd>(), line);
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

    template <fd Fd = STDOUT_FILENO, typename PrintFinalT>
    void print(PrintFinalT reqArg)
    {
        fd_to_ostream<Fd>() << reqArg;
    }
    template <fd Fd = STDOUT_FILENO, typename Required, typename... Optional>
    void print(Required reqArgs, Optional... optiArgs)
    {
        fd_to_ostream<Fd>() << reqArgs << ' ';
        print(optiArgs ...);
    }
    template <fd Fd = STDOUT_FILENO, typename... Optional>
    void println(Optional... optiArgs)
    {
        print(optiArgs ...);
        println();
    }
    template <fd Fd = STDOUT_FILENO> 
    void println()
    {
        fd_to_ostream<Fd>() << rlib::endl;
    }

    template <fd Fd = STDOUT_FILENO, typename Iterable, typename Printable>
    void print_iter(Iterable arg, Printable spliter)
    {
        for(const auto & i : arg)
            fd_to_ostream<Fd>() << i << spliter;
    }
    template <fd Fd = STDOUT_FILENO, typename Iterable, typename Printable>
    void println_iter(Iterable arg, Printable spliter)
    {
        print_iter(arg, spliter);
        fd_to_ostream<Fd>() << rlib::endl;
    }
    template <fd Fd = STDOUT_FILENO, typename Iterable>
    void print_iter(Iterable arg)
    {
        for(const auto & i : arg)
            fd_to_ostream<Fd>() << i << ' ';
    }
    template <fd Fd = STDOUT_FILENO, typename Iterable>
    void println_iter(Iterable arg)
    {
        print_iter(arg);
        fd_to_ostream<Fd>() << rlib::endl;
    }

    template <fd Fd = STDOUT_FILENO, typename... Args>
    size_t printf(const std::string &fmt, Args... args)
    {
        std::string to_print = format_string(fmt, args...); 
        fd_to_ostream<Fd>() << to_print;
        return to_print.size();
    }
    template <fd Fd = STDOUT_FILENO, typename... Args>
    size_t printfln(const std::string &fmt, Args... args)
    {
        size_t len = rlib::printf(fmt, args...);
        fd_to_ostream<Fd>() << rlib::endl;
        return len + 1;
    }
}


#endif
