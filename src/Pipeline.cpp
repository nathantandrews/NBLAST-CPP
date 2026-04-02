#include "Pipeline.hpp"
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
             const std::string& targetNeuronID) {
    std::string queryFilepath = filenameToPath(a.queryDatasetFilepath, queryNeuronID, ".swc");
    std::string strippedQuery;
    basenameNoExt(queryFilepath, strippedQuery);
    LOG_DEBUG("Query Filepath: \"%s\"", queryFilepath.c_str());
    PointVector queryVector = loadPoints(queryFilepath);

    std::string targetFilepath = filenameToPath(a.targetDatasetFilepath, targetNeuronID, ".swc");
    std::string strippedTarget;
    basenameNoExt(targetFilepath, strippedTarget);
    LOG_DEBUG("Target Filepath: \"%s\"", targetFilepath.c_str());
    PointVector targetVector = loadPoints(targetFilepath);

    return scoreNeuronPair(mat, queryVector, targetVector, a.doSine);
}

void trainMatrixStep(const Args a, 
                     const StringVector& queryFilepathVector, 
                     const StringVector& targetFilepathVector, 
                     Matrix& mat) {
    uint64_t k = queryFilepathVector.size() * drand48();
    uint64_t l = targetFilepathVector.size() * drand48();
    
    std::string queryFilepath = queryFilepathVector[k];
    LOG_DEBUG("query filepath: %s", queryFilepath.c_str());
    PointVector queryPointVector = loadPoints(queryFilepath);

    std::string targetFilepath = targetFilepathVector[l];
    LOG_DEBUG("target filepath: %s", targetFilepath.c_str());
    PointVector targetPointVector = loadPoints(targetFilepath);

    PAVector matchVector = nearestNeighborKDTree(queryPointVector, targetPointVector, a.doSine);
    for (const auto& match : matchVector) {
        if (match.queryPointID != -1 || match.targetPointID != -1) {
            mat.increment(match.distance, match.angleMeasure);
        }
   }
}

std::pair<DoubleVector, DoubleVector> generateBins(
    StringVector queryFilepathVector, 
    StringVector targetFilepathVector, 
    StringVector knownMatchesQueryVector, 
    StringVector knownMatchesTargetVector,
    unsigned numDistanceBins,
    unsigned numIters
) {
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

        std::string queryFilepath = queryFilepathVector[k];
        LOG_DEBUG("query filepath: %s", queryFilepath.c_str());
        PointVector queryPointVector = loadPoints(queryFilepath);

        std::string targetFilepath = targetFilepathVector[l];
        LOG_DEBUG("target filepath: %s", targetFilepath.c_str());
        PointVector targetPointVector = loadPoints(targetFilepath);
    
        PAVector matchVector = nearestNeighborKDTree(queryPointVector, targetPointVector, false);
        samples.insert(samples.end(), matchVector.begin(), matchVector.end());
        
        uint64_t j = knownMatchesQueryVector.size() * drand48();
        uint64_t b = knownMatchesTargetVector.size() * drand48();

        std::string knownMatchesQueryFilepath = knownMatchesQueryVector[j];
        LOG_DEBUG("query filepath: %s", knownMatchesQueryFilepath.c_str());
        PointVector knownMatchesQueryPointVector = loadPoints(knownMatchesQueryFilepath);

        std::string knownMatchesTargetFilepath = knownMatchesTargetVector[b];
        LOG_DEBUG("target filepath: %s", knownMatchesTargetFilepath.c_str());
        PointVector knownMatchesTargetPointVector = loadPoints(knownMatchesTargetFilepath);
    
        PAVector knownMatchVector = nearestNeighborKDTree(knownMatchesQueryPointVector, knownMatchesTargetPointVector, false);
        samples.insert(samples.end(), knownMatchVector.begin(), knownMatchVector.end());
       
    }
    double minDistance = std::numeric_limits<double>::max();
    double maxDistance = 0.0;
    for (auto& match : samples) {
        if (match.queryPointID == -1 || match.targetPointID == -1) {
            continue;
        }
        if (match.distance < minDistance) minDistance = match.distance;
        if (match.distance > maxDistance) maxDistance = match.distance;
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
    for (auto& elem : distanceBins) {
        LOG_DEBUG("distanceBin: %f", elem);
    }
    return std::pair(distanceBins, angleBins);
}