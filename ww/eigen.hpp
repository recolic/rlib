#ifndef RLIB_WW_EIGEN_HPP_
#define RLIB_WW_EIGEN_HPP_

#include <eigen3/Eigen/Dense>
#include <initializer_list>
#include <rlib/require/cxx14>
#include <cassert>

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
        /*     n
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
                // Warning: assume n <= MatSizeX here.
                std::memcpy(unitLPtr + unitLStep * cter, zeros.data(), unitLStep * sizeof(ScalarT));
                std::memcpy(unitLPtr + unitLStep * cter, values.data() + ((m+n-1)-(m+cter)), (m + cter) * sizeof(ScalarT));
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
            assert(_values.size() == m+n-1);
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


    }
}

#endif