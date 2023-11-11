#include <rlib/macro.hpp>
#include <rlib/string.hpp>
#include <stdexcept>
#include <cerrno>
#include <string>

namespace rlib::details {
struct die_impl {
    explicit die_impl(std::string hint_str) : msg(std::move(hint_str)) {}
    auto operator=(const long &syscall_res) const {
        if(syscall_res < 0)
            throw std::runtime_error(rlib::string("Dead. {}. System call returns {}, errno={}, strerror={}").format(msg, syscall_res, errno,
                                                                                                     strerror(errno)));
    }
    std::string msg;
};
}
#define die_on_fail rlib::details::die_impl(std::string(__FILE__ ":" RLIB_MACRO_TO_CSTR(__LINE__))) =

