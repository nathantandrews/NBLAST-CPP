#ifndef MATRIX_IO_HPP
#define MATRIX_IO_HPP

#include "Matrix.hpp"
#include <string>

namespace MatrixIO {

    Matrix loadMatrixFromTSV(const std::string& filepath);
    Matrix buildCountsMatrixFromFile(const std::string& filepath, 
                                     const std::vector<double> distanceBins, 
                                     const std::vector<double> angleBins);

} // namespace MatrixIO

#endif
