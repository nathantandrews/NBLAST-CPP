#include "FileIO.hpp"
#include "Point.hpp"
#include "Error.hpp"
#include "Matrix.hpp"

#include "Logging.hpp"

#include <fstream>
#include <filesystem>
#include <string>
#include <limits>
#include <vector>

uint64_t computeLineCount(std::ifstream& fin) {
    fin.seekg(0, std::ios::beg);
    const size_t bufferSize = 1 << 20;
    std::vector<char> buffer(bufferSize);
    unsigned lineCount = 0;
    while (fin) {
        fin.read(buffer.data(), buffer.size());
        std::streamsize bytes_read = fin.gcount();
        for (std::streamsize i = 0; i < bytes_read; ++i) {
            if (buffer[i] == '\n') ++lineCount;
        }
    }
    fin.clear();
    fin.seekg(0, std::ios::beg);
    return lineCount + 1;
}

PointVector loadPoints(const std::string& filepath) {
    std::ifstream fin{filepath};
    if (!fin) { throw std::runtime_error("Cannot open " + filepath); }
    PointVector vec;
    int pointCount = computeLineCount(fin);
    LOG_DEBUG("pointCount: %d", pointCount);
    vec.resize(pointCount);

    std::string line;
    size_t id = -1;
    while (std::getline(fin, line)) {
        if (line.empty()) continue;
        if (!line.empty() && line[0] == '#') continue;
        std::istringstream sin(line);
        sin >> id;
        Point p;
        p.parse(line);
        vec[id] = p;
    }
    fin.close();
    return vec;
}

void ensureDirectory(const std::string& filepath) {
    namespace fs = std::filesystem;

    fs::path p(filepath);
    if (p.has_parent_path()) {
        fs::create_directories(p.parent_path());
    }
}

StringVector getDatasetFilepaths(const std::string& filepath) {
    namespace fs = std::filesystem;
    std::vector<std::string> pathVector;
    for (auto const& dir_entry : fs::directory_iterator{filepath}) {
        pathVector.push_back(dir_entry.path().string());
    }
    return pathVector;
}

StringVectorPair getKnownMatchesFilepaths(const std::string& filepath) {
    StringVectorPair vecPair;
    std::ifstream fin{filepath, std::ios::in};
    if (!fin) { throw std::runtime_error("Cannot open " + filepath); }
    std::string line, ignore, query, target;
    std::istringstream sin;
    std::getline(fin, ignore); // ignore header
    while (std::getline(fin, line)) {
        sin.clear();
        sin.str(line);
        sin >> query >> target;
        if (sin.fail()) throw std::runtime_error("Malformed line in " + filepath + ": " + line);
        vecPair.first.push_back(query);
        vecPair.second.push_back(target);
        sin.str("");
    }
    return vecPair;
}


