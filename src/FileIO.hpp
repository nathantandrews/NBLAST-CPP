#ifndef FILEIO_HPP
#define FILEIO_HPP

#include "Point.hpp"

#include <vector>
#include <string>

using DoubleVector = std::vector<double>;
using DoubleVector2D = std::vector<DoubleVector>;

int loadPoints(const std::string& filepath, PointVector& vec);
int pMatrixFromFile(const std::string& filepath, DoubleVector2D& matrix);

void printMatrix(std::ofstream& out, const DoubleVector2D& matrix);
int readMatrix(const std::string& filepath, DoubleVector2D& matrix);

void ensureDirectory(const std::string& path);

int getDatasetFilepaths(const std::string& filepath, std::vector<std::string>& pathVector);

int getKnownMatchesFilepaths(const std::string& filepath, 
                    std::vector<std::string>& queryVector, 
                    std::vector<std::string>& targetVector);

#endif // FILEIO_HPP
