#ifndef SCORING_HPP
#define SCORING_HPP

#include "Matrix.hpp"
#include "Point.hpp"

PAVector nearestNeighborKDTree(const PointVector& query, 
                               const PointVector& target, 
                               bool doSine = false, 
                               bool doPrint = false);
PAVector nearestNeighborNaive(const PointVector& query, 
                              const PointVector& target, 
                              bool doSine = false, 
                              bool doPrint = false);
double scoreNeuronPair(const Matrix& mat, 
                       const PointVector& queryVector, 
                       const PointVector& targetVector, 
                       bool doSine = false);

#endif // SCORING_HPP