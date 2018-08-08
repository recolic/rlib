#include <rlib/ww/eigen.hpp>
#include <iostream>
using namespace rlib::Eigen;
using namespace ::Eigen;

int main() {
    Matrix<int, 16, 12> toInit;
    initBandMatrix<2, 3>(toInit, {1,2,3,4});
    std::cout << toInit <<std::endl<< getBandMatrix<int, 8, 14, 3, 1>({5,6,7}) << std::endl;

    Matrix<float, 10, 10> myEmptyFloatMatrix = Matrix<float, 10, 10>::Zero();
    initBandledMatrix(myEmptyFloatMatrix, std::map<int, float>{
                                                                {1, 1.0},
                                                                {4, 4.0},
                                                                {-1, 2.0},
                                                                {-5, 3.0},
                                                                {-6, 5.0}
                                                              }, 7, 6);
    std::cout << myEmptyFloatMatrix << std::endl;
    return 0;
}