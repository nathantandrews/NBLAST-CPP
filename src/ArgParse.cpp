#include "ArgParse.hpp"

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
void printUsage(std::ostream& out) { out << USAGE_MSG; }
void invalidCombinationError(const std::string& m1, const std::string& m2) {
    fprintf(stderr, INVALID_COMB_ERR_MSG, m1.c_str(), m2.c_str());
    printUsage(std::cerr);
    exit(EXIT_FAILURE);
}
void invalidArgumentError(const std::string& arg) {
    fprintf(stderr, INVALID_ARG_ERR_MSG, arg.c_str()); 
    printUsage(std::cerr);
    exit(EXIT_FAILURE);
}
void outOfRangeError(const std::string& arg) {
    fprintf(stderr, OUT_OF_RANGE_ERR_MSG, arg.c_str());
    printUsage(std::cerr);
    exit(EXIT_FAILURE);
}
void requiredArgumentError(char option) {
    fprintf(stderr, REQ_ARG_ERR_MSG, option);
    printUsage(std::cerr);
    exit(EXIT_FAILURE);
}
void invalidOptionError(char option) {
    fprintf(stderr, INVALID_OPT_ERR_MSG, option);
    printUsage(std::cerr);
    exit(EXIT_FAILURE);
}
void filepathEmptyError(const std::string& type) {
    fprintf(stderr, FILEPATH_EMPTY_ERR_MSG, type.c_str());
    printUsage(std::cerr);
    exit(EXIT_FAILURE);
}
void fileOpeningError(const std::string& type) {
    fprintf(stderr, FILE_OPENING_ERR_MSG, type.c_str());
    printUsage(std::cerr);
    exit(EXIT_FAILURE);
}

int Args::parse(int argc, char *argv[]) {
    if (argc == 1) {
        printUsage(std::cout);
        exit(EXIT_SUCCESS);
    }
    int opt = 0, rc = 0;
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
                    invalidCombinationError(mode, option::Query);
                }
                this->mode = option::Query;
                this->matrixFilepath = optarg; // needs to be changed to accept comma sep
                break;
            }
            // generator toolchain, generates match and random p-value matrices
            // given .swc files and known matches file @todo comma separated
            case 'g': {
                if (mode != option::DefaultMode) { 
                    invalidCombinationError(mode, option::GenerateECDF);
                }
                this->mode = option::GenerateECDF;
                this->knownMatchesFilepath = optarg;
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
            }
            case '?': {
                invalidOptionError(static_cast<char>(optopt));
            }
            default: {
                invalidOptionError(static_cast<char>(optopt));
            }
        }
    }
    this->optind = optind;
}