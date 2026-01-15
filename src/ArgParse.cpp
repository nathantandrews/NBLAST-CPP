#include "ArgParse.hpp"
#include "Error.hpp"

#include <string>
#include <iostream>

// C-based includes
#include <unistd.h>
#include <cstring>
#include <cassert>

std::string optToString(option m) {
    switch (m)
    {
        case option::Query: return "q";
        case option::GenerateECDF: return "g";

        case option::Random: return "r";
        case option::ComputeMatrix: return "s";
        case option::MatrixSpecified: return "m";
        case option::TimeSpecified: return "t";
        case option::DefaultMode: return "default";
        default: return "unknown";
    }
}

static std::pair<std::string, std::uint64_t> parseFilePair(const char* arg) {
    std::string s(arg);
    auto pos = s.find(',');

    if (pos == std::string::npos) {
        invalidArgumentError("'-g knownMatchesFile,#' requires a comma between the two values");
        exit(1);
    }

    std::string knownMatchesFile = s.substr(0, pos);
    std::string numItersString = s.substr(pos+1);
    if (knownMatchesFile.empty()) {
        filepathEmptyError("first filepath in -g knownMatchesFile,#");
        exit(1);
    }
    uint64_t numItersUint;
    try {
        numItersUint = std::stoul(numItersString);
    } catch (const std::invalid_argument& e) {
        invalidArgumentError("numGeneratorIterations must be an unsigned integer");
        exit(1);
    } catch (const std::out_of_range& e) {
        outOfRangeError("numGeneratorIterations");
        exit(1);
    }

    return {knownMatchesFile, numItersUint};
}

int Args::parse(int argc, char *argv[]) {
    if (argc == 1) {
        printUsage(std::cout);
        exit(EXIT_SUCCESS);
    }
    int opt = 0;
    while ((opt = getopt(argc, argv, ":hq:g:s")) != -1) {
        switch (opt) {
            // print usage
            case 'h': {
                printUsage(std::cout);
                exit(EXIT_SUCCESS);
            }
            // ===== main modes =====
            // query toolchain, compares one query .swc file to one or more target .swc files
            case 'q': {
                if (this->mode != option::DefaultMode) {
                    invalidCombinationError(optToString(mode), optToString(option::Query));
                }
                this->mode = option::Query;
                this->matrixFilepath = optarg; // needs to be changed to accept comma sep
                break;
            }
            // generator toolchain, generates match and random p-value matrices
            // given .swc files and known matches file @todo comma separated
            case 'g': {
                if (mode != option::DefaultMode) { 
                    invalidCombinationError(optToString(mode), optToString(option::GenerateECDF));
                }
                this->mode = option::GenerateECDF;

                // -g knownMatchesFile,f2
                auto [knownMatchesFile, f2] = parseFilePair(optarg);
                this->knownMatchesFilepath = knownMatchesFile;
                this->numGeneratorIterations = f2;
                
                break;
            }
            // ===== options =====
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
    }
    this->optind = optind;
    return 0;
}