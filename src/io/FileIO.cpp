#include "FileIO.hpp"
#include "core/Point.hpp"
#include "utils/ArgParse.hpp"
#include "utils/Logging.hpp"
#include "utils/StringUtils.hpp"

#include <filesystem>
#include <fstream>
#include <string>

PointVector loadPoints(const std::string &filepath) {
  std::ifstream fin{filepath};
  if (!fin) {
    throw std::runtime_error("Cannot open " + filepath);
  }
  PointVector vec;
  std::string line;
  while (std::getline(fin, line)) {
    if (line.empty())
      continue;
    if (!line.empty() && line[0] == '#')
      continue;
    Point p;
    p.parse(line);
    vec.push_back(p);
  }
  fin.close();
  return vec;
}

std::vector<Neuron>
loadNeurons(const std::vector<std::string> &neuronFilepaths) {
  std::vector<Neuron> neuronVector;
  neuronVector.reserve(neuronFilepaths.size());
  for (size_t i = 0; i < neuronFilepaths.size(); ++i) {
    neuronVector.emplace_back(neuronFilepaths.at(i));
  }
  return neuronVector;
}

void ensureDirectory(const std::string &filepath) {
  namespace fs = std::filesystem;

  fs::path p(filepath);
  if (p.has_parent_path()) {
    fs::create_directories(p.parent_path());
  }
}

StringVector getDatasetFilepaths(const std::string &filepath) {
  namespace fs = std::filesystem;
  std::vector<std::string> pathVector;
  for (auto const &dir_entry : fs::directory_iterator{filepath}) {
    pathVector.push_back(dir_entry.path().string());
  }
  return pathVector;
}

StringVectorPair getKnownMatchesFilepaths(const Args &a) {
  StringVectorPair vecPair;
  std::ifstream fin{a.knownMatchesFilepath, std::ios::in};
  if (!fin) {
    throw std::runtime_error("Cannot open " + a.knownMatchesFilepath);
  }
  std::string line, ignore, query, target;
  std::istringstream sin;
  std::getline(fin, ignore); // ignore header
  while (std::getline(fin, line)) {
    sin.clear();
    sin.str(line);
    sin >> query >> target;
    if (sin.fail())
      throw std::runtime_error("Malformed line in " + a.knownMatchesFilepath +
                               ": " + line);
    vecPair.first.push_back(
        filenameToPath(a.queryDatasetFilepath, query, ".swc"));
    vecPair.second.push_back(
        filenameToPath(a.targetDatasetFilepath, target, ".swc"));
    sin.str("");
  }
  return vecPair;
}
