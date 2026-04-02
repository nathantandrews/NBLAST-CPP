#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <vector>
#include <array>
#include <string>

// defaults for banc-fafb
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

using DoubleVector = std::vector<double>;
using DoubleVector2D = std::vector<DoubleVector>;

class Matrix {
    public:
        Matrix() : 
            distanceBins(), 
            angleBins(), 
            table() 
        {}
        Matrix(const DoubleVector& distanceBins, const DoubleVector& angleBins) : 
            distanceBins(distanceBins.begin(), distanceBins.end()), 
            angleBins(angleBins.begin(), angleBins.end()), 
            table(distanceBins.size(), 
                  DoubleVector(angleBins.size(), 0.0)) 
        {}
        void increment(double distance, double angle, double value = 1.0);
        Matrix& prefixSum();
        Matrix& toECDF();
        double score(double distance, double angle) const;
        friend std::ostream& operator<<(std::ostream& out, const Matrix& mat);
        inline DoubleVector& getDistanceBins() { return distanceBins; }
        inline DoubleVector& getAngleBins() { return angleBins; }
        inline DoubleVector2D& getTable() { return table; }
    private:
        DoubleVector distanceBins;
        DoubleVector angleBins;
        DoubleVector2D table;

        int findDistanceBin(double value) const;
        int findAngleBin(double value) const;
};

#endif // MATRIX_HPP