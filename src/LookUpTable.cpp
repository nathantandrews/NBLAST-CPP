#include "LookUpTable.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

bool LookUpTable::loadFromTSV(const std::string& filename){
    std::ifstream file(filename);
    if (!file.is_open()){
        std::cerr << "Cannot open " << filename << "\n";
        return false;
    }

    distanceBins.clear();
    angleBins.clear();
    table.clear();

    std::string line;
    bool isHeader = true;

    while (std::getline(file, line)) {
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
                angleBins.push_back(std::stod(parts[i].substr(position + 1)));
            }
            continue;
        }

        // distance bin
        double upperDst = std::stod(parts[0]);
        distanceBins.push_back(upperDst);

        std::vector<double> row;
        for (size_t i = 1; i < parts.size(); ++i) row.push_back(std::stod(parts[i]));

        table.push_back(row);
    }

    return true;
}

int LookUpTable::findBin(const std::vector<double>& bins, double value) const{
    for (size_t i = 0; i < bins.size(); ++i){
        if (value <= bins[i]) return i;
    }
    return bins.size() - 1;
}

double LookUpTable::lookUp(double distance, double angle) const{
    int row = findBin(distanceBins, distance);
    int col = findBin(angleBins, angle);
    return table[row][col];
}