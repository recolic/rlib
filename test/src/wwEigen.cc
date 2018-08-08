#include <rlib/ww/eigen.hpp>
#include <iostream>
#include <eigen3/Eigen/Sparse>
using namespace rlib::Eigen;
using namespace ::Eigen;

int main() {
    Matrix<int, 16, 12> toInit;
    initBandMatrix<2, 3>(toInit, {1,2,3,4});
    std::cout << toInit <<std::endl<< getBandMatrix<int, 8, 14, 3, 1>({5,6,7}) << std::endl;

    Matrix<float, 10, 10> myEmptyFloatMatrix = Matrix<float, 10, 10>::Zero();
    auto testingMap = std::map<int, float>{
      {1, 1.0},
      {4, 4.0},
      {-1, 2.0},
      {-5, 3.0},
      {-6, 5.0}
    };
    initBundledMatrix(myEmptyFloatMatrix, testingMap, 7, 6);
    std::cout << myEmptyFloatMatrix << "\n" << getBundledMatrix<MatrixXf>(testingMap, 7, 6) << '\n' << getBundledMatrix<Matrix<float,7,6>>(testingMap) << std::endl;
    std::cout << "hi\n" << getBundledMatrix<SparseMatrix<float>>(testingMap, 7, 6) << std::endl;
    return 0;
}