#ifndef MATRIX_IO_HPP
#define MATRIX_IO_HPP

#include "Matrix.hpp"
#include <string>

namespace MatrixIO {

    Matrix loadMatrixFromTSV(const std::string& filepath);
    Matrix buildCountsMatrixFromFile(const std::string& filepath);

} // namespace MatrixIO

#endif
