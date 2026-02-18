#include "MatrixIO.hpp"
#include "Matrix.hpp"

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>

namespace MatrixIO {

    Matrix loadMatrixFromTSV(const std::string& filepath) {
        std::ifstream fin(filepath);
        if (!fin.is_open()){
            throw std::runtime_error("Cannot open " + filepath);
        }
        Matrix m;

        std::string line;
        bool isHeader = true;

        while (std::getline(fin, line)) {
            std::stringstream ss(line);
            std::string cell;
            // store cells
            std::vector<std::string> parts;

            while (std::getline(ss, cell, '\t')) parts.push_back(cell);

            // angle bin
            if (isHeader) {
                isHeader = false;
                for (size_t i = 1; i < parts.size(); ++i) {
                    // cos_0.1 (if sin will use the same format?)
                    auto position = parts[i].find('_');
                    m.getAngleBins().push_back(std::stod(parts[i].substr(position + 1)));
                }
                continue;
            }

            // distance bin
            double upperDst = std::stod(parts[0]);
            m.getDistanceBins().push_back(upperDst);

            std::vector<double> row;
            for (size_t i = 1; i < parts.size(); ++i) row.push_back(std::stod(parts[i]));

            m.getTable().push_back(row);
        }

        return m;
    }
    Matrix buildCountsMatrixFromFile(const std::string& filepath) {
        std::ifstream fin(filepath);
        if (!fin.is_open()) { throw std::runtime_error("Cannot open " + filepath); }
        Matrix mat(DISTANCE_BINS, ANGLE_BINS);
        std::string line;
        while (std::getline(fin, line)) {
            const char* ptr = line.c_str();
            char* end;
            double distance = std::strtod(ptr, &end);
            double angle = std::strtod(end, nullptr);
            mat.increment(distance, angle);
        }
        return mat;
    }

}
