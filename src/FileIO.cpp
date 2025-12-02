#include "FileIO.hpp"
#include "Point.hpp"
#include "Utils.hpp"

#include <fstream>
#include <string>
#include <limits>

int loadPoints(const std::string& filepath, PointVector& vec) {
    std::ifstream fin{filepath};
    if (!fin) { return -1; }
    
    int pointCount = computeLineCount(fin);
    vec.reserve(pointCount);

    std::string line;
    unsigned id = -1;
    while (std::getline(fin, line)) {
        if (line.empty()) continue;
        if (!line.empty() && line[0] == '#') continue;
        std::istringstream sin(line);
        sin >> id;
        Point p;
        p.parse(line);
        vec[id] = p;
    }
    vec.shrink_to_fit();
    fin.close();
    return 0;
}
int pMatrixFromFile(const std::string& filepath, DoubleVector2D& matrix) {
    std::ifstream fin{filepath, std::ios::in};
    if (!fin) { fileOpeningError(filepath); }
    std::string line, ignore;
    std::istringstream sin;
    double distance, theta;
    int scaled_dist, scaled_ang;
    std::getline(fin, ignore); // first line is the two filenames
    while (std::getline(fin, line)) {
        sin.clear();
        sin.str(line);
        sin >> ignore >> ignore;
        if (sin.fail()) continue;
        sin >> distance >> theta;
        scaled_dist = (int) (sqrt(distance));
        scaled_ang = (int) (sqrt(MATRIX_THETA_SCALING_FACTOR * theta));
        ++matrix[scaled_dist][scaled_ang];
        sin.str("");
    }
    countsToPValueMatrix(matrix);
    return 0;
}

int readMatrix(const std::string& filepath, DoubleVector2D& matrix) {
    std::ifstream fin{filepath, std::ios::in};
    if (!fin) {
        std::cerr << "Error: Cannot open stream with filepath: '" 
            << filepath << "'." << std::endl;
        return -1;
    }
    std::string str;
    size_t i, j;
    while (fin >> str) {
        if (str == "#") {
            fin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        } else {
            break;
        }
    }
    matrix[0][0] = std::stoi(str);
    for (size_t i = 0; i < matrix.size(); ++i) {
        for (size_t j = 0; j < matrix[i].size(); ++j) {
            fin >> matrix[i][j];
        }
    }
    return 0;
}

// @todo actually print out some useful info about the matrix.
void printMatrixHeader(size_t matrix_i_size, size_t matrix_j_size) {
    printf("# %lu x %lu P-Value Matrix!\n", matrix_i_size, matrix_j_size);
}

void printMatrix(const DoubleVector2D& matrix) {
    printMatrixHeader(matrix.size(), matrix[0].size());
    for (size_t i = 0; i < matrix.size(); ++i) {
        for (size_t j = 0; j < matrix[i].size(); ++j) {
            std::cout << matrix[i][j];
            if (j < matrix[i].size() - 1) {
                std::cout << " ";
            }
        }
        std::cout << "\n";
    }
}