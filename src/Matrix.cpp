#include "Matrix.hpp"

#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>

void Matrix::increment(double distance, double angle, double value) {
    int row = findDistanceBin(distance);
    int col = findAngleBin(angle);
    table[row][col] += value;
}
void Matrix::prefixSum() {
    for (size_t i = 0; i < table.size(); ++i) {
        int tmp = 0, row_counter = 0;
        for (size_t j = 0; j < table[i].size(); ++j) {
            tmp = table[i][j];
            table[i][j] += row_counter;
            row_counter += tmp;
            if (i > 0) {
                table[i][j] += table[i - 1][j];
            }
        }
    }
}
void Matrix::toECDF() {
    if (table.empty() || table[0].empty()) throw std::runtime_error("cannot convert to ECDF: matrix empty/invalid");

    double total = table.back().back();
    if (total == 0.0) throw std::runtime_error("cannot convert to ECDF: total sum of matrix is zero");

    for (size_t i = 0; i < table.size(); ++i) {
        for (size_t j = 0; j < table[i].size(); ++j) {
            table[i][j] /= total;
        }
    }
}

double Matrix::score(double distance, double angle) const {
    int row = findDistanceBin(distance);
    int col = findAngleBin(angle);
    return table.at(row).at(col);
}
std::ostream& operator<<(std::ostream& out, const Matrix& mat) {
    constexpr int precision = 4;

    std::ios oldState(nullptr);
    oldState.copyfmt(out);


    out << "dist/angle";
    out << "\t";
    for (size_t i = 0; i < mat.angleBins.size(); ++i) {
        out << "cos_" << mat.angleBins[i];
        if (i + 1 < mat.angleBins.size())
            out << "\t";
    }
    out << "\n";

    for (size_t j = 0; j < mat.table.size(); ++j) {
        out << std::fixed << std::setprecision(0);
        out << mat.distanceBins[j] << "\t";
        out << std::fixed << std::setprecision(precision);

        for (size_t k = 0; k < mat.table[j].size(); ++k) {
            out << mat.table[j][k];
            if (k + 1 < mat.table[j].size())
                out << "\t";
        }
        out << "\n";
    }

    out.copyfmt(oldState);

    return out;
}


int Matrix::findDistanceBin(double value) const {
    for (size_t i = 0; i < distanceBins.size(); ++i){
        if (value <= distanceBins[i]) return i;
    }
    return distanceBins.size() - 1;
}
int Matrix::findAngleBin(double value) const {
    for (size_t i = 0; i < angleBins.size(); ++i){
        if (value <= angleBins[i]) return i;
    }
    return angleBins.size() - 1;
}
