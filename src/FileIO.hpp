#ifndef FILEIO_HPP
#define FILEIO_HPP

#include "Point.hpp"

#include <string>

using DoubleVector = std::vector<double>;
using DoubleVector2D = std::vector<DoubleVector>;

int loadPoints(const std::string& filepath, PointVector& vec);
int pMatrixFromFile(const std::string& filepath, DoubleVector2D& matrix);

void printMatrix(const DoubleVector2D& matrix);
int readMatrix(const std::string& filepath, DoubleVector2D& matrix);

void ensureDirectory(const std::string& path);

#endif // FILEIO_HPP
