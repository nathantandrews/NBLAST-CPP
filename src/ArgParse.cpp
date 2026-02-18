#include "ArgParse.hpp"
#include "Error.hpp"
#include "Logging.hpp"
#include "StringUtils.hpp"

#include <string>
#include <iostream>

// C-based includes
#include <unistd.h>
#include <cstring>
#include <cassert>

extern int optind;

std::ostream& operator<<(std::ostream& out, option_t op) {
    switch (op) {
        case option_t::Query: out << 'q'; break;
        case option_t::GenerateScoringMatrix: out << 'g'; break;
        case option_t::MatrixSpecified: out << 'm'; break;
        case option_t::DefaultMode: out << "default"; break;
        default: out << "Unknown"; break;
    }
    return out;
}

std::string optToString(option_t m) {
    switch (m)
    {
        case option_t::Query: return "q";
        case option_t::GenerateScoringMatrix: return "g";
        case option_t::MatrixSpecified: return "m";
        case option_t::InputDirectoriesSpecified: return "i";
        case option_t::DefaultMode: return "default";
        default: return "unknown";
    }
}

std::ostream& operator<<(std::ostream& out, const Args& a) {
    out << "optind: " << a.optind << '\n'
        << "matrixFilepath: " << a.matrixFilepath << '\n'
        << "knownMatchesFilepath: " << a.knownMatchesFilepath << '\n'
        << "queryDatasetFilepath: " << a.queryDatasetFilepath << '\n'
        << "targetDatasetFilepath: " << a.targetDatasetFilepath << '\n'
        << "mode: " << a.mode << '\n'
        << "numGeneratorIterations: " << a.numGeneratorIterations << '\n'
        << "doSine: " << a.doSine;
    return out;
}

void Args::setMode(option_t next) {
    if (mode != option_t::DefaultMode) {
        throw std::runtime_error("Invalid combination: " + optToString(mode) + " and " + optToString(next));
    }
    mode = next;
}

int Args::parse(int argc, char *argv[]) {
    if (argc == 1) {
        printUsage(std::cout);
        exit(EXIT_SUCCESS);
    }
    int opt = 0;
    bool optIProvided = false;
    while ((opt = getopt(argc, argv, ":hq:g:i:sl:")) != -1) {
        switch (opt) {
            // print usage
            case 'h': { printUsage(std::cout); exit(EXIT_SUCCESS); }
            // ===== main modes =====
            // query toolchain, compares one query .swc file to one or more target .swc files
            case 'q': {
                setMode(option_t::Query);
                matrixFilepath = optarg;
                if (matrixFilepath.empty()) {
                    throw std::runtime_error("matrixFilepath cannot be empty");
                }
                break;
            }
            // generator toolchain, generates match and random p-value matrices
            // given .swc files and known matches file
            case 'g': {
                setMode(option_t::GenerateScoringMatrix);

                // comma separated arg
                std::pair<std::string, std::string> res;
                int rc = splitOnComma(optarg, res);
                if (rc) {
                    throw std::runtime_error("argument for -g invalid");
                }

                if (res.first.empty()) {
                    throw std::runtime_error("filepath in -g option empty");
                }
                uint64_t numItersUint;
                rc = stringToUInt(res.second, numItersUint);
                if (rc == -1) {
                    throw std::runtime_error("numGeneratorIterations must be an unsigned integer");
                } else if (rc == -2) {
                    throw std::runtime_error("numGeneratorIterations out of range");
                }
                knownMatchesFilepath = res.first;
                numGeneratorIterations = numItersUint;
                
                break;
            }
            // ===== options =====
            // input directories
            case 'i': {
                optIProvided = true;
                std::pair<std::string, std::string> res;
                int rc = splitOnComma(optarg, res);
                if (rc) {
                    throw std::runtime_error("-g queryDataset,targetDataset invalid");
                }
                if (res.first.empty()) {
                    throw std::runtime_error("query dataset filepath empty");
                } else if (res.second.empty()) {
                    throw std::runtime_error("target dataset filepath empty");
                }
                queryDatasetFilepath = res.first;
                targetDatasetFilepath = res.second;
                
                break;
            }
            case 's': { doSine = true; break; }
            // case 'l' {
            //     break;
            // }
            case ':': {
                throw std::runtime_error(std::string("option requires an argument -") + static_cast<char>(optopt)); break;
            }
            case '?': {
                throw std::runtime_error(std::string("option is invalid -") + static_cast<char>(optopt)); break;
            }
        }
    }

    if (mode == option_t::Query && !optIProvided) {
        throw std::runtime_error("The -q option requires -i to specify query and target datasets.");
    } else if (mode == option_t::GenerateScoringMatrix && !optIProvided) {
        throw std::runtime_error("The -g option requires -i to specify query and target datasets.");
    }

    this->optind = ::optind;
    return ::optind;
}