#include "Runner.hpp"
#include "ArgParse.hpp"
#include "Matrix.hpp"
#include "MatrixIO.hpp"
#include "Error.hpp"
#include "FileIO.hpp"
#include "Scoring.hpp"
#include "Logging.hpp"
#include "StringUtils.hpp"
#include "Timer.hpp"
#include "Pipeline.hpp"

#include <iostream>

void runQueryMode(const Args& a) {
    LOG_INFO("Using Scoring Matrix: \"%s\"", a.matrixFilepath.c_str());
        
    Matrix mat = MatrixIO::loadMatrixFromTSV(a.matrixFilepath);
    std::string queryNeuronID, targetNeuronID;
    TimerStats ts;
    if (a.positionalArgs.empty()) {
        while (std::cin >> queryNeuronID >> targetNeuronID) {
            double score = timeFunction(ts, [&](){ 
                return query(a, mat, queryNeuronID, targetNeuronID); 
            });
            std::cout << queryNeuronID << "\t" 
                    << targetNeuronID << "\t" 
                    << score << "\n";
        }
        std::ofstream tout("query-times.txt");
        ts.print(tout);
        tout.close();
        return;
    }
    queryNeuronID = a.positionalArgs[0];
    for (size_t i = 1; i < a.positionalArgs.size(); i++) {
        targetNeuronID = a.positionalArgs[i];

        double score = query(a, mat, queryNeuronID, targetNeuronID);

        std::cout << queryNeuronID << "\t" 
                    << targetNeuronID << "\t" 
                    << score << "\n";
    }
    std::cout.flush();
}

void runGeneratorMode(const Args& a) {
    LOG_DEBUG("grabbing swc filepaths for query dataset...");
    StringVector queryFilepathVector = getDatasetFilepaths(a.queryDatasetFilepath);
    LOG_DEBUG("query dataset size: %d", queryFilepathVector.size());
    
    LOG_DEBUG("grabbing swc filepaths for target dataset...");
    StringVector targetFilepathVector = getDatasetFilepaths(a.targetDatasetFilepath);
    LOG_DEBUG("target dataset size: %d", targetFilepathVector.size());

    LOG_DEBUG("getting known matches filepaths from %s", a.knownMatchesFilepath.c_str());
    auto [knownMatchesQueryVector, knownMatchesTargetVector] = getKnownMatchesFilepaths(a);
    LOG_DEBUG("known matches: query size = %d, target size = %d", 
        knownMatchesQueryVector.size(), knownMatchesQueryVector.size());
    
    auto [distanceBins, angleBins] = generateBins(queryFilepathVector, 
                                                  targetFilepathVector, 
                                                  knownMatchesQueryVector, 
                                                  knownMatchesTargetVector, 
                                                  10, 
                                                  5
                                                );

    Matrix knownMatrix(distanceBins, angleBins);
    Matrix randomMatrix(distanceBins, angleBins);
    size_t iters = a.numGeneratorIterations;
    while(iters > 0) {
        LOG_DEBUG("iterations left %d", a.numGeneratorIterations);
        --iters;
        
        // known matches
        LOG_DEBUG("starting known match");
        trainMatrixStep(a, knownMatchesQueryVector, knownMatchesTargetVector, knownMatrix);

        // random matches
        LOG_DEBUG("starting random match");
        trainMatrixStep(a, queryFilepathVector, targetFilepathVector, randomMatrix);
    }

    if (a.doDump) {
        // ensures directory out is there once (don't have to do again)
        ensureDirectory("out/knownCounts.tsv");
        std::ofstream kcout("out/knownCounts.tsv");
        kcout << knownMatrix;
        kcout.close();
                    
        knownMatrix.prefixSum().toECDF();
        std::ofstream keout("out/knownECDF.txt");
        keout << knownMatrix;
        keout.close();

        std::ofstream rcout("out/randomCounts.tsv");
        rcout << randomMatrix;
        rcout.close();
        
        randomMatrix.prefixSum().toECDF();
        std::ofstream reout("out/randomECDF.txt");
        reout << randomMatrix;
        reout.close();
    } else {
        knownMatrix.prefixSum().toECDF();
        randomMatrix.prefixSum().toECDF();
    }
    
    Matrix logLikelihoodMatrix(distanceBins, angleBins);
    const double epsilon = 1e-12;
    for (size_t i = 0; i < logLikelihoodMatrix.getTable().size(); ++i) {
        for (size_t j = 0; j < logLikelihoodMatrix.getTable()[i].size(); ++j) {
            logLikelihoodMatrix.getTable()[i][j] = std::log2(knownMatrix.getTable()[i][j] / (randomMatrix.getTable()[i][j] + epsilon));
        }
    }
    if (!a.matrixOutfile.empty()) {
        std::ofstream mout(a.matrixOutfile);
        mout << logLikelihoodMatrix;
        mout.close();
    } else {
        std::cout << logLikelihoodMatrix;
    }

}

int run(const Args& a) {
    switch (a.mode) {
        // query two neurons for given datasets, 
        // print resulting score to stdout unless specified
        case option_t::Query: {
            runQueryMode(a);
            break;
        }
        // generate a matrix for given datasets, 
        // print it to stdout unless specified
        case option_t::GenerateScoringMatrix: {
            runGeneratorMode(a);
            break;
        }
        default: { throw std::runtime_error("uncaught argument parsing error, invalid mode"); }
    }
    return 0;
}
