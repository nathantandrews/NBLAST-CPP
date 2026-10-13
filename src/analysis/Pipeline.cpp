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

// Self-scores for every neuron; each is independent so this parallelises trivially.
static std::vector<double> allSelfScores(const Matrix &mat,
                                         const std::vector<Neuron> &neurons) {
  std::vector<double> selfScores(neurons.size());
#pragma omp parallel for schedule(dynamic)
  for (long i = 0; i < (long)neurons.size(); ++i)
    selfScores[i] = neurons[i].selfScore(mat);
  return selfScores;
}

// score(i,j) == score(j,i), so only the upper triangle is computed and mirrored.
// Each pair is written by exactly one thread and no sums cross threads, so the
// result is identical for any thread count.
std::vector<Score> allByAll(const Matrix &mat,
                            const std::vector<Neuron> &neurons) {
  const size_t n = neurons.size();
  std::vector<double> selfScores = allSelfScores(mat, neurons);

  std::vector<std::pair<size_t, size_t>> upper;
  upper.reserve(n * (n + 1) / 2);
  for (size_t i = 0; i < n; ++i)
    for (size_t j = i; j < n; ++j)
      upper.emplace_back(i, j);

  std::vector<double> scores(n * n);
#pragma omp parallel for schedule(dynamic, 16)
  for (long k = 0; k < (long)upper.size(); ++k) {
    size_t i = upper[k].first, j = upper[k].second;
    double s = neurons[i].score(neurons[j], mat, selfScores[i], selfScores[j]);
    scores[i * n + j] = s;
    scores[j * n + i] = s;
  }

  std::vector<Score> scoreVector;
  scoreVector.reserve(n * n);
  for (size_t i = 0; i < n; ++i)
    for (size_t j = 0; j < n; ++j)
      scoreVector.push_back({neurons[i].getNID(), neurons[j].getNID(), scores[i * n + j]});
  return scoreVector;
}

// Scores arbitrary (query, target) index pairs. Self-scores are computed once per
// neuron that is actually used, not once per pair.
std::vector<Score> queryPairs(const Matrix &mat,
                              const std::vector<Neuron> &neurons,
                              const std::vector<std::pair<size_t, size_t>> &pairs) {
  std::vector<char> used(neurons.size(), 0);
  for (const auto &p : pairs)
    used[p.first] = used[p.second] = 1;
  std::vector<double> selfScores(neurons.size(), 0.0);
#pragma omp parallel for schedule(dynamic)
  for (long i = 0; i < (long)neurons.size(); ++i)
    if (used[i])
      selfScores[i] = neurons[i].selfScore(mat);

  std::vector<Score> out(pairs.size());
#pragma omp parallel for schedule(dynamic, 16)
  for (long k = 0; k < (long)pairs.size(); ++k) {
    size_t i = pairs[k].first, j = pairs[k].second;
    out[k] = {neurons[i].getNID(), neurons[j].getNID(),
              neurons[i].score(neurons[j], mat, selfScores[i], selfScores[j])};
  }
  return out;
}

std::vector<PAVector> calcDistsDotprods(const std::vector<Neuron> &queryNeurons,
                                         const std::vector<Neuron> &targetNeurons,
                                         const std::vector<std::pair<size_t, size_t>> &subset,
                                         bool ignoreSelf) {
  std::vector<PAVector> results;
  results.reserve(subset.size());

  for (const auto &pair : subset) {
    size_t q_idx = pair.first;
    size_t t_idx = pair.second;

    if (ignoreSelf && queryNeurons[q_idx].getNID() == targetNeurons[t_idx].getNID()) {
      continue;
    }

    PAVector cop = queryNeurons[q_idx].nearestNeighbors(targetNeurons[t_idx]);
    results.push_back(cop);
  }
  return results;
}

void updateMatrixWithResults(Matrix &mat, const std::vector<PAVector> &results) {
  for (const auto &matchVector : results) {
    for (const auto &match : matchVector) {
      if (match.queryPointID != -1 && match.targetPointID != -1) {
        mat.increment(match.distance, match.angleMeasure);
      }
    }
  }
}

std::pair<DoubleVector, DoubleVector> generateBins(
    StringVector queryFilepathVector, StringVector targetFilepathVector,
    StringVector knownMatchesQueryVector, StringVector knownMatchesTargetVector,
    double binFactor, unsigned numIters, double coordScale) {
  if (binFactor <= 1.0) {
    throw std::runtime_error("binFactor must be > 1.0");
  } else if (numIters == 0) {
    throw std::runtime_error("numIters cannot be 0");
  }
  DoubleVector distanceBins;
  DoubleVector angleBins;
  PAVector samples;
  // Use all known matches for range estimation
  LOG_INFO("Sampling known match pairs for binning...");
  for (size_t i = 0; i < knownMatchesQueryVector.size(); ++i) {
    Neuron query = Neuron(knownMatchesQueryVector[i], coordScale);
    Neuron target = Neuron(knownMatchesTargetVector[i], coordScale);
    PAVector matchVector = query.nearestNeighbors(target);
    samples.insert(samples.end(), matchVector.begin(), matchVector.end());
  }

  // Sample random matches for range estimation
  LOG_INFO("Sampling %u random pairs for binning...", numIters);
  for (unsigned i = 0; i < numIters; ++i) {
    uint64_t qi = queryFilepathVector.size() * drand48();
    uint64_t ti = targetFilepathVector.size() * drand48();

    Neuron query = Neuron(queryFilepathVector[qi], coordScale);
    Neuron target = Neuron(targetFilepathVector[ti], coordScale);
    PAVector matchVector = query.nearestNeighbors(target);
    samples.insert(samples.end(), matchVector.begin(), matchVector.end());
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
  double effectiveMin = std::max(minDistance, 0.25);
  double effectiveMax = std::max(maxDistance, effectiveMin + 1.0);
  LOG_DEBUG("effectiveMin: %f", effectiveMin);
  LOG_DEBUG("effectiveMax: %f", effectiveMax);

  // Build geometric series: effectiveMin, effectiveMin*factor, effectiveMin*factor^2, ...
  double logFactor = std::log(binFactor);
  unsigned numDistanceBins = static_cast<unsigned>(
      std::ceil(std::log(effectiveMax / effectiveMin) / logFactor));
  LOG_INFO("Generated %u distance bins (factor=%.3f, range=[%.2f, %.2f])",
           numDistanceBins, binFactor, effectiveMin, effectiveMax);
  for (unsigned i = 0; i <= numDistanceBins; ++i) {
    double proposedDistanceBin = effectiveMin * std::pow(binFactor, i);
    if (proposedDistanceBin <= 64) {
      distanceBins.push_back(proposedDistanceBin);
    }
  }

  angleBins.insert(angleBins.end(), ANGLE_BINS.begin(), ANGLE_BINS.end());
  return std::pair(distanceBins, angleBins);
}

Matrix calcScoreMatrix(const Matrix &matchProb, const Matrix &randProb,
                       double logbase, double epsilon) {
  if (matchProb.getDistanceBins() != randProb.getDistanceBins() ||
      matchProb.getAngleBins() != randProb.getAngleBins()) {
    throw std::runtime_error("calcScoreMatrix: match and rand matrices must "
                             "have identical bins.");
  }

  Matrix scoreMat(matchProb.getDistanceBins(), matchProb.getAngleBins());
  const auto &matchTable = matchProb.getTable();
  const auto &randTable = randProb.getTable();
  auto &scoreTable = scoreMat.getTable();

  double logOfBase = std::log(logbase);

  for (size_t i = 0; i < scoreTable.size(); ++i) {
    for (size_t j = 0; j < scoreTable[i].size(); ++j) {
      double num = matchTable[i][j] + epsilon;
      double den = randTable[i][j] + epsilon;
      scoreTable[i][j] = std::log(num / den) / logOfBase;
    }
  }

  return scoreMat;
}
