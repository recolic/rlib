#include <rlib/ww/eigen.hpp>
#include <iostream>
using namespace rlib::Eigen;
using namespace ::Eigen;

int main() {
    Matrix<int, 16, 12> toInit;
    initBandMatrix<2, 3>(toInit, {1,2,3,4});
    std::cout << toInit <<std::endl<< getBandMatrix<int, 8, 14, 3, 1>({5,6,7}) << std::endl;
    return 0;
}