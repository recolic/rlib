#ifndef RLIB_WW_EIGEN_HPP_
#define RLIB_WW_EIGEN_HPP_

#include <eigen3/Eigen/Dense>
#include <initializer_list>
#include <rlib/require/cxx14>
#include <cassert>
#include <array>
#include <map>

namespace rlib {
    namespace impl {
        template <int row, int col, int _opt, int dyRow, int dyCol> struct eigenMatExtracterHelper {
            static constexpr int _y = row == ::Eigen::Dynamic ? dyRow : row;
            static constexpr int _x = col == ::Eigen::Dynamic ? dyCol : col;
        };
        template <typename T>
        struct eigenMatExtracter {
            static constexpr int x = 0;
            static constexpr int y = 0;
            using scalarType = T;
        };
        template <template<typename, int...> class MatType, typename ScalarT, int... Args>
        struct eigenMatExtracter <MatType<ScalarT, Args...> > {
            static constexpr int x = eigenMatExtracterHelper<Args ...>::_x;
            static constexpr int y = eigenMatExtracterHelper<Args ...>::_y;
            using scalarType = ScalarT;
        };
    }
    namespace Eigen {
        // Band matrix here. I'll generate it by memcpy, dense only. If you use Dynamic matrix,
        //     you must set MaxRow&&MaxCol to its exact size, and disable alignment. Usually, it's efficent for small dense matrix.
        // I'll fill zero and all useful elements.
        /* Example:
         *     n
         *  /---|
         * m|2 1 0 0 0 0
         *  L3 2 1 0 0 0
         *   0 3 2 1 0 0
         *   0 0 3 2 1 0
         *   0 0 0 3 2 1
         *   0 0 0 0 3 2
         *   Here, ScalarT is int, nullScalar is default(0), values is {1,2,3}.
         *   Here, unitA is '1 2 3 0 0 0 0' at col2
         */
        // Warning: If ScalaT has copy constructor, it won't be called.
        //          ScalarT must be able to copy by std::memcpy.
        template <size_t m, size_t n, typename MatT, typename ScalarT, bool disableDynamicCheck = false>
        void initBandMatrix(MatT &fixedMatRef, const std::array<ScalarT, m+n-1> &values, ScalarT nullScalar = ScalarT{}) {
            using wrappedMat = rlib::impl::eigenMatExtracter<MatT>;
            static_assert(std::is_same<typename wrappedMat::scalarType, ScalarT>::value,
                "Element of std::array and Eigen::Matrix must be the same.");
            constexpr int MatSizeX = wrappedMat::x;
            constexpr int MatSizeY = wrappedMat::y;
            static_assert(disableDynamicCheck || (MatSizeX != ::Eigen::Dynamic && MatSizeY != ::Eigen::Dynamic),
                "initBandMatrix must not be applied on Dynamic Matrix.");
            static_assert(MatSizeX >= n, "MatSizeX MUST >= n");
            static_assert(MatSizeY >= m, "MatSizeY MUST >= m");

 
            ScalarT *cmat = fixedMatRef.data(); // Fortran-style matrix. I must reverse m/n below.
            // unitA replica area refers to `First column with ALL m+n-1 values in argument`.
            // Right area named unitR, and left area named unitL
            constexpr int unitLStep = MatSizeY;
            constexpr int unitLCols = n - 1;
            constexpr int unitAStep = MatSizeY + 1;
            constexpr int unitAReplica = std::min(MatSizeY-m-n+1, MatSizeX-n/*Matrix maybe too "narrow"*/);
            constexpr int unitACols = unitAStep * unitAReplica / MatSizeY;
            constexpr int unitAShift = unitLCols * MatSizeY;
            constexpr int unitRStep = MatSizeY;
            constexpr int unitRCols = MatSizeX - (unitACols + unitLCols);
            constexpr int unitRShift = MatSizeY * (unitACols + unitLCols);
            ScalarT *unitLPtr = cmat;
            ScalarT *unitAPtr = cmat + unitAShift;
            ScalarT *unitRPtr = cmat + unitRShift; 
            std::array<ScalarT, unitRStep> zeros{nullScalar};
            // unitL
            for(int cter = 0; cter < unitLCols; ++cter) {
                std::memcpy(unitLPtr + unitLStep * cter, zeros.data(), unitLStep * sizeof(ScalarT));
                std::memcpy(unitLPtr + unitLStep * cter, values.data() + ((m+n-1)-(m+cter)), std::min(m+cter, (size_t)unitLStep) * sizeof(ScalarT));
            }
            // unitA, hot
            std::array<ScalarT, unitAStep> unitAMem {nullScalar};
            std::memcpy(unitAMem.data(), values.data(), sizeof(ScalarT) * (m+n-1));
            for(int cter = 0; cter < unitAReplica; ++cter) {
                std::memcpy(unitAPtr + (unitAStep * cter), unitAMem.data(), unitAStep * sizeof(ScalarT));
            }
            // unitR
            for(int cter = 0; cter < unitRCols; ++cter) {
                std::memcpy(unitRPtr + (unitRStep * cter), zeros.data(), unitRStep * sizeof(ScalarT));
                int copyRangeBegin = std::min(unitACols + unitLCols - n + 1 + cter, (size_t)MatSizeY);
                int copyRangeEnd = std::min(m+n-1 + copyRangeBegin, (size_t)MatSizeY);
                int copyRangeLen = copyRangeEnd - copyRangeBegin;
                if(!copyRangeLen) continue; // Must fill other space with null.
                std::memcpy(unitRPtr + (unitRStep * cter) + copyRangeBegin, values.data(), copyRangeLen * sizeof(ScalarT));
            }
        }
        template <size_t m, size_t n, typename MatT, typename ScalarT, bool disableDynamicCheck = false>
        void initBandMatrix(MatT &fixedMatRef, std::initializer_list<ScalarT> _values, ScalarT nullScalar = ScalarT{}) {
            assert(_values.size() == m+n-1 && ("Initializer_list MUST have a correct size(m+n-1)", true));
            std::array<ScalarT, m+n-1> values;
            std::copy(_values.begin(), _values.end(), values.begin());
            initBandMatrix<m, n, MatT, ScalarT, disableDynamicCheck>(fixedMatRef, values, nullScalar);
        }
 
        template <typename ScalarT, int MatSizeY, int MatSizeX> struct EigenMatrixStackLimitChecker {
#if EIGEN_STACK_ALLOCATION_LIMIT
            //static constexpr bool value = ::Eigen::internal::check_static_allocation_size<ScalarT, MatSizeX*MatSizeY>();
            // Eigen::internal implemented this feature without checking alignment size. So I won't check it.
            static constexpr bool value = sizeof(ScalarT) * MatSizeX * MatSizeY <= EIGEN_STACK_ALLOCATION_LIMIT;
#else
            static constexpr bool value = true;
#endif
            using resultType = typename std::conditional<value, ::Eigen::Matrix<ScalarT, MatSizeY, MatSizeX>, 
                ::Eigen::Matrix<ScalarT, ::Eigen::Dynamic, ::Eigen::Dynamic, ::Eigen::DontAlign | ::Eigen::ColMajor, MatSizeY, MatSizeX>>::type;
            
        };
        template <typename ScalarT, int MatSizeY, int MatSizeX, size_t m, size_t n>
        typename EigenMatrixStackLimitChecker<ScalarT, MatSizeY, MatSizeX>::resultType
            getBandMatrix(std::initializer_list<ScalarT> _values) {
            using ResultT = typename EigenMatrixStackLimitChecker<ScalarT, MatSizeY, MatSizeX>::resultType;

            ResultT result;
            initBandMatrix<m, n, ResultT, ScalarT, true>(result, std::move(_values));
            return std::move(result);
        }
        template <typename ScalarT, int MatSizeY, int MatSizeX, size_t m, size_t n>
        typename EigenMatrixStackLimitChecker<ScalarT, MatSizeY, MatSizeX>::resultType
            getBandMatrix(const std::array<ScalarT, m+n-1> &values) {
            using ResultT = typename EigenMatrixStackLimitChecker<ScalarT, MatSizeY, MatSizeX>::resultType;

            ResultT result;
            initBandMatrix<m, n, ResultT, ScalarT, true>(result, values);
            return std::move(result);
        }

        // Bandled matrix here. Generated by a simple while loop. **I am not filling zero**.
        // It has much lower performance on dense matrix compared with initBandMatrix.
        //     but it usually has better performance on sparse matrix.
        /* Example(index rule):
         *Idx 0   1   4
         *    \  |    \
         *     0 1 0 0 4 0
         * -1->2 0 1 0 0 4
         *     0 2 0 1 0 0
         * -3->0 0 2 0 1 0
         *     0 0 0 2 0 1
         *     3 0 0 0 2 0 -> 0
         *     5 3 0 0 0 2
         *    /      |    \      
         *  -6      -3     -1
         * How to construct this example bundled matrix:
         *     It has 5 ropes, so just provide their index and value:
         * initBandledMatrix(myEmptyFloatMatrix, std::map<int, float>{
         *                                                             {1, 1.0},
         *                                                             {4, 4.0},
         *                                                             {-1, 2.0},
         *                                                             {-5, 3.0},
         *                                                             {-6, 5.0}
         *                                                           });
         * In C++17, you may use better std::map to pleasure yourself:)
         * 
         * In bandledMatrix, there's no need to know matSize at compile time, because compiler may do little thing
         *     to optimize a magic, long, for loop, compared with the RBtree access time...
         */
        template <typename RandomAccessedMatT, typename ScalarT>
        void initBandledMatrix(RandomAccessedMatT &matRef, const std::map<int, ScalarT> &ropes, size_t matSizeY, size_t matSizeX) {
            // Be calm... Do not play with C++17
            for(const auto &pair : ropes) {
                int index = pair.first;
                const ScalarT &val = pair.second;
                assert(index > 0-(int)matSizeY && index < (int)matSizeX && ("Index of rope must NOT exceed the matrix area.", true));
                // cache unfriendly but what if it's a sparse matrix? so it doesn't matter.
                size_t cterX = (index>0?index:0);
                size_t cterY = (index<0?0-index:0);
                while(true) {
                    if(cterX == matSizeX || cterY == matSizeY)
                        break;
                    matRef(cterY, cterX) = val;
                    ++cterX;
                    ++cterY;
                }
            }
        }
    }
}

#endif