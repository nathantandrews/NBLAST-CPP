#ifndef PIPELINE_HPP
#define PIPELINE_HPP

#include "core/Matrix.hpp"
#include "core/Neuron.hpp"
#include "utils/ArgParse.hpp"

#include <string>

typedef struct Score {
  std::string qid, tid;
  double score;
  void print(std::ostream &out, const std::string &sep) const;
} Score;

Score query(const Matrix &mat, const Neuron &query, const Neuron &target);

std::vector<Score> allByAll(const Matrix &mat,
                            const std::vector<Neuron> &neurons);

void trainMatrixStep(Matrix &mat, const StringVector &queryFilepathVector,
                     const StringVector &targetFilepathVector);
using DoubleVector = std::vector<double>;
std::pair<DoubleVector, DoubleVector> generateBins(
    StringVector queryFilepathVector, StringVector targetFilepathVector,
    StringVector knownMatchesQueryVector, StringVector knownMatchesTargetVector,
    unsigned numDistanceBins, unsigned numIters);

#endif // PIPELINE_HPP
