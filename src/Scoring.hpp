#ifndef SCORING_HPP
#define SCORING_HPP

#include "Matrix.hpp"
#include "Point.hpp"

PAVector nearestNeighborKDTree(const PointVector& query, 
                               const PointVector& target, 
                               bool doCosine, 
                               bool doPrint);
PAVector nearestNeighborNaive(const PointVector& query, 
                              const PointVector& target, 
                              bool doCosine, 
                              bool doPrint);
double scoreNeuronPair(const Matrix& mat, 
                       const PointVector& queryVector, 
                       const PointVector& targetVector, 
                       bool doCosine);

#endif // SCORING_HPP