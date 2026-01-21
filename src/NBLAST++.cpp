#include "ArgParse.hpp"
#include "LookUpTable.hpp"
#include "Error.hpp"
#include "Utils.hpp"
#include "FileIO.hpp"
#include "Scoring.hpp"
#include "Debug.hpp"

#include <iostream>
#include <cassert>

#include <unistd.h>

int main(int argc, char *argv[]) {
    Args a;
    a.parse(argc, argv);
    int rc;
    switch (a.mode) {
        case option_t::Query: {
            if (a.matrixFilepath.empty()) { 
                filepathEmptyError("Scoring Matrix");
            }

            std::cerr << "Scoring Matrix: " << a.matrixFilepath << std::endl;

            LookUpTable lut;
            lut.loadFromTSV(a.matrixFilepath);
                        
            PointVector queryVector;
            std::string queryFilepath = argv[a.optind];
            rc = loadPoints(queryFilepath, queryVector);
            assert(rc == 0);
            std::string strippedQuery = basenameNoExt(queryFilepath);
            std::cerr << "Query: " << strippedQuery << std::endl;

            for(int i = a.optind + 1; i < argc; i++) {
                std::string targetFilepath = argv[i];
                std::string strippedTarget = basenameNoExt(targetFilepath);
                std::cerr << "targetFilepath: " << targetFilepath << std::endl;
                if (targetFilepath.empty()) { std::cerr << "can't open " << strippedTarget << ".swc; continuing\n"; continue; }
                PointVector targetVector;
                rc = loadPoints(targetFilepath, targetVector);
                if (rc) { std::cerr << "failed to load points from " << strippedTarget << ".swc; continuing\n"; continue; }
                std::cerr << "Target: " << strippedTarget << std::endl;
                
                debug("scoring %s %s\n", strippedQuery.c_str(), strippedTarget.c_str());
                double score = scoreNeuronPair(lut, queryVector, targetVector, a.doSine);
                std::cout << strippedQuery << " " << strippedTarget << " " << score << "\n";
            }
            std::cout.flush();
            return 0;
        }
        // generate a matrix for a given dataset, print it to stdout
        case option_t::GenerateECDF: {
            // @todo finish mode
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
