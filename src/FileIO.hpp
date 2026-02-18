#ifndef FILEIO_HPP
#define FILEIO_HPP

#include "Point.hpp"

#include <vector>
#include <string>

PointVector loadPoints(const std::string& filepath);

void ensureDirectory(const std::string& path);

using StringVector = std::vector<std::string>;
StringVector getDatasetFilepaths(const std::string& filepath);

using StringVectorPair = std::pair<std::vector<std::string>, std::vector<std::string>>;
StringVectorPair getKnownMatchesFilepaths(const std::string& filepath);

#endif // FILEIO_HPP
