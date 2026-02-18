#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <vector>
#include <array>
#include <string>

static constexpr unsigned int NUM_DISTANCE_BINS = 7;
static constexpr unsigned int NUM_ANGLE_BINS = 10;
static constexpr std::array<double, NUM_DISTANCE_BINS> DISTANCE_BINS{ 
    20000,
    30000,
    45000,
    65000,
    90000,
    120000,
    160000
};
static constexpr std::array<double, NUM_ANGLE_BINS> ANGLE_BINS{ 
    .1, 
    .2, 
    .3, 
    .4, 
    .5,
    .6,
    .7,
    .8,
    .9,
    1
};

class Matrix {
    public:
        Matrix() : 
            distanceBins(), 
            angleBins(), 
            table() 
        {}
        Matrix(const std::array<double, NUM_DISTANCE_BINS>& distanceBins, const std::array<double, NUM_ANGLE_BINS>& angleBins) : 
            distanceBins(distanceBins.begin(), distanceBins.end()), 
            angleBins(angleBins.begin(), angleBins.end()), 
            table(NUM_DISTANCE_BINS, 
                  std::vector<double>(NUM_ANGLE_BINS, 0.0)) 
        {}
        void increment(double distance, double angle, double value = 1.0);
        void prefixSum();
        void toECDF();
        double score(double distance, double angle) const;
        friend std::ostream& operator<<(std::ostream& out, const Matrix& mat);
        inline std::vector<double>& getDistanceBins() { return distanceBins; }
        inline std::vector<double>& getAngleBins() { return angleBins; }
        inline std::vector<std::vector<double>>& getTable() { return table; }
    private:
        std::vector<double> distanceBins;
        std::vector<double> angleBins;
        std::vector<std::vector<double>> table;

        int findDistanceBin(double value) const;
        int findAngleBin(double value) const;
};

#endif // MATRIX_HPP