#include "ArgParse.hpp"
#include "Error.hpp"
#include "Logging.hpp"
#include "Utils.hpp"

#include <string>
#include <iostream>

// C-based includes
#include <unistd.h>
#include <cstring>
#include <cassert>

extern int optind;

std::string optToString(option_t m) {
    switch (m)
    {
        case option_t::Query: return "q";
        case option_t::GenerateECDF: return "g";
        case option_t::MatrixSpecified: return "m";
        case option_t::DefaultMode: return "default";
        default: return "unknown";
    }
}

int Args::parse(int argc, char *argv[]) {
    if (argc == 1) {
        printUsage(std::cout);
        exit(EXIT_SUCCESS);
    }
    int opt = 0;
    int opt_index = 1;
    while ((opt = getopt(argc, argv, ":hq:g:i:s")) != -1) {
        switch (opt) {
            // print usage
            case 'h': {
                printUsage(std::cout);
                exit(EXIT_SUCCESS);
            }
            // ===== main modes =====
            // query toolchain, compares one query .swc file to one or more target .swc files
            case 'q': {
                if (this->mode != option_t::DefaultMode) {
                    invalidCombinationError(optToString(mode), optToString(option_t::Query));
                }
                this->mode = option_t::Query;
                this->matrixFilepath = optarg; // needs to be changed to accept comma sep
                break;
            }
            // generator toolchain, generates match and random p-value matrices
            // given .swc files and known matches file @todo comma separated
            case 'g': {
                if (mode != option_t::DefaultMode) { 
                    invalidCombinationError(optToString(mode), optToString(option_t::GenerateECDF));
                }
                this->mode = option_t::GenerateECDF;

                // comma separated arg
                std::pair<std::string, std::string> res;
                int rc = splitOnComma(optarg, res);
                if (rc) {
                    invalidArgumentError("knownMatchesFile,#");
                    exit(1);
                }

                std::string knownMatchesFile = res.first;
                std::string numItersString = res.second;
                if (knownMatchesFile.empty()) {
                    filepathEmptyError("first filepath in -g knownMatchesFile,#");
                    exit(1);
                }
                uint64_t numItersUint;
                rc = stringToUInt(numItersString, numItersUint);
                if (rc == -1) {
                    invalidArgumentError("numGeneratorIterations must be an unsigned integer");
                    exit(1);
                } else if (rc == -2) {
                    outOfRangeError("numGeneratorIterations");
                    exit(1);
                }
                this->knownMatchesFilepath = knownMatchesFile;
                this->numGeneratorIterations = numItersUint;
                
                break;
            }
            // ===== options =====
            // input directories
            case 'i': {
                if (mode != option_t::GenerateECDF) { 
                    invalidCombinationError("i", optToString(option_t::GenerateECDF));
                }

                std::pair<std::string, std::string> res; 
                int rc = splitOnComma(optarg, res);
                if (rc) {
                    invalidArgumentError("queryDataset,targetDataset");
                }
                this->queryDatasetFilepath = res.first;
                this->targetDatasetFilepath = res.second;
                
                break;
            }
            // use sine instead of cosine
            case 's': {
                this->doSine = true;
                break;
            }
            case ':': {
                requiredArgumentError(static_cast<char>(optopt));
                break;
            }
            case '?': {
                invalidOptionError(static_cast<char>(optopt));
                break;
            }
            default: {
                invalidOptionError(static_cast<char>(optopt));
                break;
            }
        }
        opt_index += 1;
    }
    this->optind = opt_index + 1;
    return 0;
}