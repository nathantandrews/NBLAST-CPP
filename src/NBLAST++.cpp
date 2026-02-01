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
    LOG_INFO("Logging to %s", getLoggerConfig().filepath.c_str());
    ensureDirectory("out");
    
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
            
            ensureDirectory("out");
            std::ofstream outfile("out/generator-results.txt");
            std::ostream& out = outfile;

            
            uint64_t n = argc - a.optind; // number of input SWC files, from which random pairs will be chosen
            std::string queryFilepath, targetFilepath;

            while(a.numGeneratorIterations > 0) {
                --a.numGeneratorIterations;
                uint64_t i = a.optind + n * drand48(), j = a.optind + n * drand48();

                queryFilepath = argv[i];
                PointVector queryVector;
                rc = loadPoints(queryFilepath, queryVector);
                if (rc == -1) continue;

                targetFilepath = argv[j];
                PointVector targetVector;
                rc = loadPoints(targetFilepath, targetVector);
                if (rc == -1) continue;

                out << basenameNoExt(queryFilepath) << " " << basenameNoExt(targetFilepath) << "\n";
                PAVector matchVector = nearestNeighborKDTree(queryVector, targetVector, a.doSine, true);
            }
            out.flush();

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
