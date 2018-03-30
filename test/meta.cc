#include <rlib/meta.hpp>

int main() {
    static_assert(rlib::array_utils<int>::at<1, 33,34,35,36,37>::value() == 34);
}
