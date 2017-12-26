#ifndef _SRC_FSTR_H
#define _SRC_FSTR_H 1

#include <rlib/require/cxx11>

namespace rlib {
    template<typename... Args>
    std::string format_string_c()(const std::string &fmt, Args... args);
    template<typename... Args>
    std::string format_string()(const std::string &fmt, Args... args);
}
#endif //SRC_FSTR_H
