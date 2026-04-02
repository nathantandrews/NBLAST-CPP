#ifndef PIPELINE_HPP
#define PIPELINE_HPP

#include "ArgParse.hpp"
#include "Matrix.hpp"
#include "StringUtils.hpp"
#include "FileIO.hpp"
#include "Logging.hpp"
#include "Point.hpp"
#include "Scoring.hpp"

#include <string>

double query(const Args& a, 
             const Matrix& mat, 
             const std::string& queryNeuronID, 
             const std::string& targetNeuronID);
             
void trainMatrixStep(const Args a, 
                     const StringVector& queryFilepathVector, 
                     const StringVector& targetFilepathVector, 
                     Matrix& mat);
using DoubleVector = std::vector<double>;
std::pair<DoubleVector, DoubleVector> generateBins(
    StringVector queryFilepathVector, 
    StringVector targetFilepathVector, 
    StringVector knownMatchesQueryVector, 
    StringVector knownMatchesTargetVector,
    unsigned numDistanceBins,
    unsigned numIters
);

#endif // PIPELINE_HPP 