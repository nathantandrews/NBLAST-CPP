#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>

constexpr uint64_t DISTANCE_BIN_COUNT = 1000;
constexpr uint64_t ANGLE_BIN_COUNT = 1000;
constexpr uint64_t MATRIX_THETA_SCALING_FACTOR = 1000000;

std::string basename(const std::string& filePath);
std::string basenameNoExt(const std::string& filePath);

unsigned computeLineCount(std::ifstream& fin);

#endif // UTILS_HPP
