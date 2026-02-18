#include "ArgParse.hpp"
#include "Matrix.hpp"
#include "MatrixIO.hpp"
#include "Error.hpp"
#include "FileIO.hpp"
#include "Scoring.hpp"
#include "Logging.hpp"
#include "StringUtils.hpp"

#include <iostream>
#include <cassert>

#include <unistd.h>

int main(int argc, char *argv[]) {
    openLogFile("log/run");
#ifdef DDEBUG
    getLoggerConfig().level = LogLevel::debug;
#endif
    LOG_INFO("Logging to %s", getLoggerConfig().filepath.c_str());
    
    Args a;
    a.parse(argc, argv);
    switch (a.mode) {
        case option_t::Query: {
            LOG_INFO("Using Scoring Matrix: \"%s\"", a.matrixFilepath.c_str());

            Matrix mat = MatrixIO::loadMatrixFromTSV(a.matrixFilepath);

            std::string queryFilepath = a.queryDatasetFilepath + "/" + argv[a.optind] + ".swc";
            std::string strippedQuery;
            basenameNoExt(queryFilepath, strippedQuery);

            LOG_DEBUG("Query Filepath: \"%s\"", queryFilepath.c_str());

            PointVector queryVector = loadPoints(queryFilepath);

            for(int i = a.optind + 1; i < argc; i++) {
                std::string targetFilepath = a.targetDatasetFilepath + "/" + argv[i] + ".swc";
                std::string strippedTarget;
                basenameNoExt(targetFilepath, strippedTarget);
                
                LOG_DEBUG("Target Filepath: \"%s\"", targetFilepath.c_str());
                
                PointVector targetVector = loadPoints(targetFilepath);
                
                double score = scoreNeuronPair(mat, 
                                               queryVector, 
                                               targetVector, 
                                               a.doSine);
                std::cout << strippedQuery << "\t" 
                    << strippedTarget << "\t" 
                    << score << "\n";
            }
            std::cout.flush();
            return 0;
        }
        // generate a matrix for a given dataset, print it to stdout
        case option_t::GenerateScoringMatrix: {
#ifdef DEBUG
            srand48(1234); // seed the random number generator
#else            
            srand48(time(0) + getpid()); // seed the random number generator
#endif
            LOG_DEBUG("creating output directory");
            
            // ensures directory out is there once (don't have to do again)
            ensureDirectory("out/known-matches-data.tsv");
            // std::ofstream kout("out/known-matches-data.tsv");
            // std::ofstream rout("out/random-matches-data.tsv");

            LOG_DEBUG("grabbing swc filepaths for query dataset...");
            StringVector queryFilepaths = getDatasetFilepaths(a.queryDatasetFilepath);
            std::size_t queryFilepathsLen = queryFilepaths.size();
            LOG_DEBUG("query dataset size: %d", queryFilepathsLen);
            LOG_DEBUG("grabbing swc filepaths for target dataset...");
            StringVector targetFilepaths = getDatasetFilepaths(a.targetDatasetFilepath);
            std::size_t targetFilepathsLen = targetFilepaths.size();
            LOG_DEBUG("target dataset size: %d", targetFilepathsLen);

            LOG_DEBUG("getting known matches filepaths from %s", a.knownMatchesFilepath.c_str());
            const auto [knownMatchesQueryVector, knownMatchesTargetVector] = getKnownMatchesFilepaths(a.knownMatchesFilepath);
            std::size_t knownMatchesQueryLen = knownMatchesQueryVector.size();
            std::size_t knownMatchesTargetLen = knownMatchesQueryVector.size();
            LOG_DEBUG("known matches: query size = %d, target size = %d", knownMatchesQueryLen, knownMatchesTargetLen);

            std::string queryFilepath, targetFilepath;
            PointVector queryVector, targetVector;
            Matrix knownMatrix(DISTANCE_BINS, ANGLE_BINS);
            Matrix randomMatrix(DISTANCE_BINS, ANGLE_BINS);
            while(a.numGeneratorIterations > 0) {
                LOG_DEBUG("iterations left %d", a.numGeneratorIterations);
                --a.numGeneratorIterations;
                // known matches
                LOG_DEBUG("starting known match");
                uint64_t k = knownMatchesQueryLen * drand48();
                uint64_t l = knownMatchesTargetLen * drand48();
                
                queryFilepath = knownMatchesQueryVector[k];
                LOG_DEBUG("query: %s.swc", queryFilepath.c_str());
                queryVector.clear();
                queryVector = std::move(loadPoints(a.queryDatasetFilepath + "/" + queryFilepath + ".swc"));

                targetFilepath = knownMatchesTargetVector[l];
                LOG_DEBUG("target: %s.swc", targetFilepath.c_str());
                targetVector.clear();
                targetVector = std::move(loadPoints(a.targetDatasetFilepath + "/" + targetFilepath + ".swc"));

                std::string strippedQuery, strippedTarget; 
                basenameNoExt(queryFilepath, strippedQuery);
                basenameNoExt(targetFilepath, strippedTarget);
                // kout << strippedQuery << "\t" << strippedTarget << "\n";
                PAVector knownMatchVector = nearestNeighborKDTree(queryVector, targetVector, a.doSine, false);
                for (const auto& match : knownMatchVector) {
                    // kout << match.distance << "\t" << match.angleMeasure << "\n";
                    knownMatrix.increment(match.distance, match.angleMeasure);
                }

                // random matches
                LOG_DEBUG("starting random match");
                uint64_t i = queryFilepathsLen * drand48();
                uint64_t j = targetFilepathsLen * drand48();

                queryFilepath = queryFilepaths[i];
                LOG_DEBUG("query: %s", queryFilepath.c_str());
                queryVector.clear();
                try {
                    queryVector = std::move(loadPoints(queryFilepath));
                } catch (...) {}

                targetFilepath = targetFilepaths[j];
                LOG_DEBUG("target: %s", targetFilepath.c_str());
                targetVector.clear();
                try {
                    targetVector = std::move(loadPoints(targetFilepath));
                } catch (...) {}

                // rout << basenameNoExt(queryFilepath) << "\t" << basenameNoExt(targetFilepath) << "\n";
                PAVector randomMatchVector = nearestNeighborKDTree(queryVector, targetVector, a.doSine, false);
                for (const auto& match : randomMatchVector) {
                    // rout << match.distance << "\t" << match.angleMeasure << "\n";
                    randomMatrix.increment(match.distance, match.angleMeasure);
                }
            }
            // kout.flush();
            // rout.flush();

            // Matrix knownMatrix = MatrixIO::buildCountsMatrixFromFile("out/known-matches-data.tsv");
            
            std::ofstream kcout("out/knownCounts.tsv");
            kcout << knownMatrix;
            kcout.close();
                        
            knownMatrix.prefixSum();
            knownMatrix.toECDF();
            std::ofstream keout("out/knownECDF.txt");
            keout << knownMatrix;
            keout.close();

            // Matrix randomMatrix = MatrixIO::buildCountsMatrixFromFile("out/random-matches-data.tsv");

            std::ofstream rcout("out/randomCounts.tsv");
            rcout << randomMatrix;
            rcout.close();
            
            randomMatrix.prefixSum();
            randomMatrix.toECDF();
            std::ofstream reout("out/randomECDF.txt");
            reout << randomMatrix;
            reout.close();

            Matrix logLikelihoodMatrix(DISTANCE_BINS, ANGLE_BINS);
            const double epsilon = 1e-12;
            for (size_t i = 0; i < logLikelihoodMatrix.getTable().size(); ++i) {
                for (size_t j = 0; j < logLikelihoodMatrix.getTable()[i].size(); ++j) {
                    logLikelihoodMatrix.getTable()[i][j] = std::log2(knownMatrix.getTable()[i][j] / randomMatrix.getTable()[i][j] + epsilon);
                }
            }
            std::ofstream mout("out/matrix.tsv");
            mout << logLikelihoodMatrix;
            return 0;
        }
        default: { throw std::runtime_error("uncaught argument parsing error, invalid mode"); }
    }
}
