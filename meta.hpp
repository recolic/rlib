#ifndef RLIB_META_HPP_
#define RLIB_META_HPP_

#include <rlib/require/cxx17>
#include <cstddef> // size_t

namespace rlib {
    template <typename T>
    struct array_utils {
        template <size_t index, T first_ele, T... arr>
        struct at_last {
            static constexpr T value() {
                if constexpr(sizeof...(arr) == index)
                    return first_ele;
                else
                    return at_last<index, arr ...>::value();
            }
        };

        template <size_t index, T... arr>
        struct at {
            static constexpr T value() {
                return at_last<sizeof...(arr) - index - 1, arr ...>::value();
            }
        };
    };
}

#endif
