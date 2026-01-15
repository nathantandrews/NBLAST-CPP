#ifndef LOOKUPTABLE_HPP
#define LOOKUPTABLE_HPP

#include <vector>
#include <string>

class LookUpTable {
    public: 
        bool loadFromTSV(const std::string& filename);
        double lookUp(double distance, double angle) const;

    private:
        std::vector<double> distanceBins;
        std::vector<double> angleBins;
        std::vector<std::vector<double>> table;

        int findBin(const std::vector<double>& bins, double value) const;
};

#endif