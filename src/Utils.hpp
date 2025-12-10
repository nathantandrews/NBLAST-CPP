#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <vector>

using DoubleVector = std::vector<double>;
using DoubleVector2D = std::vector<DoubleVector>;

constexpr uint64_t DISTANCE_BIN_COUNT = 1000;
constexpr uint64_t ANGLE_BIN_COUNT = 1000;
constexpr uint64_t MATRIX_THETA_SCALING_FACTOR = 1000000;

std::string basename(const std::string& filePath);
std::string basenameNoExt(const std::string& filePath);

unsigned computeLineCount(std::ifstream& fin);

int countsToPValueMatrix(DoubleVector2D& matrix);

#endif // UTILS_HPP
