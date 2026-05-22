#include "Pipeline.hpp"
#include "core/Matrix.hpp"
#include "core/Point.hpp"
#include "utils/ArgParse.hpp"
#include "utils/Logging.hpp"

#include <cmath>
#include <limits>
#include <string>

void Score::print(std::ostream &out, const std::string &sep) const {
  out << qid << sep << tid << sep << score << std::endl;
}

Score query(const Matrix &mat, const Neuron &query, const Neuron &target) {
  double score = query.score(target, mat);
  Score s{query.getNID(), target.getNID(), score};
  return s;
}

std::vector<Score> allByAll(const Matrix &mat,
                            const std::vector<Neuron> &neurons) {
  std::vector<Score> scoreVector;
  scoreVector.reserve(neurons.size() * neurons.size());
  for (size_t i = 0; i < neurons.size(); ++i) {
    for (size_t j = i; j < neurons.size(); ++j) {
      scoreVector.push_back(query(mat, neurons[i], neurons[j]));
    }
  }
  return scoreVector;
}

void trainMatrixStep(Matrix &mat, const StringVector &queryFilepathVector,
                     const StringVector &targetFilepathVector) {
  std::vector<Neuron> queryNeurons = loadNeurons(queryFilepathVector);
  std::vector<Neuron> targetNeurons = loadNeurons(targetFilepathVector);
  uint64_t k = queryNeurons.size() * drand48();
  uint64_t l = targetNeurons.size() * drand48();

  PAVector matchVector = queryNeurons[k].nearestNeighbors(targetNeurons[l]);
  for (const auto &match : matchVector) {
    if (match.queryPointID != -1 || match.targetPointID != -1) {
      mat.increment(match.distance, match.angleMeasure);
    }
  }
}

std::pair<DoubleVector, DoubleVector> generateBins(
    StringVector queryFilepathVector, StringVector targetFilepathVector,
    StringVector knownMatchesQueryVector, StringVector knownMatchesTargetVector,
    unsigned numDistanceBins, unsigned numIters) {
  if (numDistanceBins == 0) {
    throw std::runtime_error("numDistanceBins cannot be 0");
  } else if (numIters == 0) {
    throw std::runtime_error("numIters cannot be 0");
  }
  DoubleVector distanceBins;
  DoubleVector angleBins;
  PAVector samples;
  for (unsigned i = 0; i < numIters; ++i) {
    uint64_t k = queryFilepathVector.size() * drand48();
    uint64_t l = targetFilepathVector.size() * drand48();

    Neuron query = Neuron(queryFilepathVector[k]);
    Neuron target = Neuron(targetFilepathVector[l]);
    PAVector matchVector = query.nearestNeighbors(target);
    samples.insert(samples.end(), matchVector.begin(), matchVector.end());

    uint64_t j = knownMatchesQueryVector.size() * drand48();
    uint64_t b = knownMatchesTargetVector.size() * drand48();

    Neuron knownMatchQuery = Neuron(knownMatchesQueryVector[j]);
    Neuron knownMatchTarget = Neuron(knownMatchesTargetVector[b]);
    PAVector knownMatchVector =
        knownMatchQuery.nearestNeighbors(knownMatchTarget);
    samples.insert(samples.end(), knownMatchVector.begin(),
                   knownMatchVector.end());
  }
  double minDistance = std::numeric_limits<double>::max();
  double maxDistance = 0.0;
  for (auto &match : samples) {
    if (match.queryPointID == -1 || match.targetPointID == -1) {
      continue;
    }
    if (match.distance < minDistance)
      minDistance = match.distance;
    if (match.distance > maxDistance)
      maxDistance = match.distance;
  }
  LOG_DEBUG("minDistance: %f", minDistance);
  LOG_DEBUG("maxDistance: %f", maxDistance);
  double epsilon = 1e-12;
  double logMin = std::log(std::max(minDistance, epsilon));
  double logMax = std::log(maxDistance);
  LOG_DEBUG("logMin: %f", logMin);
  LOG_DEBUG("logMax: %f", logMax);
  for (size_t i = 0; i <= numDistanceBins; ++i) {
    double t = static_cast<double>(i) / numDistanceBins;
    distanceBins.push_back(std::exp(logMin + t * (logMax - logMin)));
  }

  angleBins.insert(angleBins.end(), ANGLE_BINS.begin(), ANGLE_BINS.end());
  return std::pair(distanceBins, angleBins);
}
