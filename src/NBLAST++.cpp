#include "ArgParse.hpp"
#include "LookUpTable.hpp"
#include "Error.hpp"
#include "Utils.hpp"
#include "FileIO.hpp"
#include "Scoring.hpp"
#include "Debug.hpp"
#include "Logging.hpp"

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
    int rc;
    switch (a.mode) {
        case option_t::Query: {
            if (a.matrixFilepath.empty()) { 
                filepathEmptyError("Scoring Matrix");
            }

            LOG_INFO("Using Scoring Matrix: \"%s\"", a.matrixFilepath.c_str());

            LookUpTable lut;
            lut.loadFromTSV(a.matrixFilepath);

            std::string queryFilepath = argv[a.optind];
            std::string strippedQuery = basenameNoExt(queryFilepath);

            LOG_DEBUG("Query Filepath: \"%s\"", queryFilepath.c_str());
            LOG_INFO("Query: \"%s\"", strippedQuery.c_str());

            PointVector queryVector;
            rc = loadPoints(queryFilepath, queryVector);
            if (rc) {
                    LOG_ERROR("Failed to load points from \"%s\"; exiting...", queryFilepath.c_str());
                    exit(EXIT_FAILURE);
            }

            ensureDirectory("out");
            std::ofstream outfile("out/query-results.tsv");
            std::ostream& out = outfile;

            for(int i = a.optind + 1; i < argc; i++) {
                std::string targetFilepath = argv[i];
                std::string strippedTarget = basenameNoExt(targetFilepath);
                
                LOG_DEBUG("Target Filepath: \"%s\"", targetFilepath.c_str());
                LOG_INFO("Target: \"%s\"", strippedTarget.c_str());
                
                PointVector targetVector;
                rc = loadPoints(targetFilepath, targetVector);
                if (rc) { 
                    LOG_WARN("Failed to load points from \"%s\"; continuing...", targetFilepath.c_str());
                    continue; 
                }
                
                double score = scoreNeuronPair(lut, 
                                               queryVector, 
                                               targetVector, 
                                               a.doSine);
                out << strippedQuery << "\t" 
                    << strippedTarget << "\t" 
                    << score << "\n";
            }
            out.flush();
            return 0;
        }
        // generate a matrix for a given dataset, print it to stdout
        case option_t::GenerateECDF: {
#ifdef DEBUG
            srand48(1234); // seed the random number generator
#else            
            srand48(time(0) + getpid()); // seed the random number generator
#endif
            LOG_DEBUG("creating output directory");
            ensureDirectory("out/known-matches-data.tsv");
            std::ofstream knownMatchesOutfile("out/known-matches-data.tsv");
            std::ostream& kout = knownMatchesOutfile;
            ensureDirectory("out/random-matches-data.tsv");
            std::ofstream randomMatchesOutfile("out/random-matches-data.tsv");
            std::ostream& rout = randomMatchesOutfile;

            if (a.queryDatasetFilepath.empty()) {
                filepathEmptyError("Query Dataset");
            } else if (a.targetDatasetFilepath.empty()) {
                filepathEmptyError("Target Dataset");
            }

            LOG_DEBUG("grabbing swc filepaths for query dataset...");
            std::vector<std::string> queryDataFilepaths;
            getDatasetFilepaths(a.queryDatasetFilepath, queryDataFilepaths);
            std::size_t queryDataLen = queryDataFilepaths.size();
            LOG_DEBUG("query dataset size: %d", queryDataLen);

            LOG_DEBUG("grabbing swc filepaths for target dataset...");
            std::vector<std::string> targetDataFilepaths;
            getDatasetFilepaths(a.targetDatasetFilepath, targetDataFilepaths);
            std::size_t targetDataLen = targetDataFilepaths.size();
            LOG_DEBUG("target dataset size: %d", targetDataLen);

            std::vector<std::string> knownMatchesQueryVector;
            std::vector<std::string> knownMatchesTargetVector;
            LOG_DEBUG("getting known matches filepaths from %s", a.knownMatchesFilepath.c_str());
            getKnownMatchesFilepaths(a.knownMatchesFilepath, knownMatchesQueryVector, knownMatchesTargetVector);
            std::size_t knownMatchesQueryLen = knownMatchesQueryVector.size();
            std::size_t knownMatchesTargetLen = knownMatchesQueryVector.size();
            LOG_DEBUG("known matches: query size = %d, target size = %d", knownMatchesQueryLen, knownMatchesTargetLen);

            std::string queryFilepath, targetFilepath;
            PointVector queryVector, targetVector;
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
                rc = loadPoints(a.queryDatasetFilepath + "/" + queryFilepath + ".swc", queryVector);
                if (rc == -1) continue;

                targetFilepath = knownMatchesTargetVector[l];
                LOG_DEBUG("target: %s.swc", targetFilepath.c_str());
                targetVector.clear();
                rc = loadPoints(a.targetDatasetFilepath + "/" + targetFilepath + ".swc", targetVector);
                if (rc == -1) continue;

                kout << basenameNoExt(queryFilepath) << "\t" << basenameNoExt(targetFilepath) << "\n";
                PAVector knownMatchVector = nearestNeighborKDTree(queryVector, targetVector, a.doSine, false);
                for (const auto& match : knownMatchVector) {
                    kout << match.distance << "\t" << match.angleMeasure << "\n";
                }

                // random matches
                LOG_DEBUG("starting random match");
                uint64_t i = queryDataLen * drand48();
                uint64_t j = targetDataLen * drand48();

                queryFilepath = queryDataFilepaths[i];
                LOG_DEBUG("query: %s", queryFilepath.c_str());
                queryVector.clear();
                rc = loadPoints(queryFilepath, queryVector);
                if (rc == -1) continue;

                targetFilepath = targetDataFilepaths[j];
                LOG_DEBUG("target: %s", targetFilepath.c_str());
                targetVector.clear();
                rc = loadPoints(targetFilepath, targetVector);
                if (rc == -1) continue;

                // rout << basenameNoExt(queryFilepath) << "\t" << basenameNoExt(targetFilepath) << "\n";
                PAVector randomMatchVector = nearestNeighborKDTree(queryVector, targetVector, a.doSine, false);
                for (const auto& match : randomMatchVector) {
                    rout << match.distance << "\t" << match.angleMeasure << "\n";
                }
            }
            kout.flush();
            rout.flush();

            DoubleVector2D knownMatrix(1000, DoubleVector(1000, 0.0));
            rc = pMatrixFromFile("out/known-matches-data.tsv", knownMatrix);
            if (rc) {
                LOG_ERROR("Error when binning known matches data");
            }
            
            ensureDirectory("out/knownBinned.txt");
            std::ofstream knownBinned("out/knownBinned.txt");
            printMatrix(knownBinned, knownMatrix);
            
            rc = countsToPValueMatrix(knownMatrix);
            if (rc) {
                LOG_ERROR("Error when converting to p values in known matches matrix");
            }
            
            ensureDirectory("out/knownPValues.txt");
            std::ofstream knownPValues("out/knownPValues.txt");
            printMatrix(knownPValues, knownMatrix);
            
            DoubleVector2D randomMatrix(1000, DoubleVector(1000, 0.0));
            rc = pMatrixFromFile("out/random-matches-data.tsv", randomMatrix);
            if (rc) {
                LOG_ERROR("Error when binning random matches data");
            }

            ensureDirectory("out/randomBinned.txt");
            std::ofstream randomBinned("out/randomBinned.txt");
            printMatrix(randomBinned, knownMatrix);

            rc = countsToPValueMatrix(randomMatrix);
            if (rc) {
                LOG_ERROR("Error when converting to p values in random matches matrix");
            }

            ensureDirectory("out/randomPValues.txt");
            std::ofstream randomPValues("out/randomPValues.txt");
            printMatrix(randomPValues, knownMatrix);

            DoubleVector2D eCDF(1000, DoubleVector(1000, 0.0));
            for (size_t i = 0; i < 1000; ++i) {
                for (size_t j = 0; j < 1000; ++j) {
                    eCDF[i][j] = std::log2(knownMatrix[i][j] / randomMatrix[i][j] + 0.00000000001);
                }
            }
            ensureDirectory("out/matrix.txt");
            std::ofstream matrixFile("out/matrix.txt");
            printMatrix(matrixFile, eCDF);
            return 0;
        }
        // first step in generating the matrix
        // randomly selects pairs of .swc files,
        // and outputs nearest neighbor calculations to stdout
        case option_t::Random: {
            uint64_t n = argc - a.optind; // number of input SWC files, from which random pairs will be chosen
            std::string queryFilepath, targetFilepath;
            srand48(time(0) + getpid()); // seed the random number generator
            while(a.numGeneratorIterations > 0) {
                uint64_t i = a.optind + n * drand48(), j = a.optind + n * drand48();

                queryFilepath = argv[i];
                PointVector queryVector;
                rc = loadPoints(queryFilepath, queryVector);
                if (rc == -1) continue;

                targetFilepath = argv[j];
                PointVector targetVector;
                rc = loadPoints(targetFilepath, targetVector);
                if (rc == -1) continue;

                std::cout << basenameNoExt(queryFilepath) << " " << basenameNoExt(targetFilepath) << "\n";
                PAVector matchVector = nearestNeighborKDTree(queryVector, targetVector, a.doSine, true);
            }
            return 0;
        }
        // in case something went wrong in argument parsing
        default: { invalidArgumentError(optToString(a.mode)); return -1; }
    }
}
