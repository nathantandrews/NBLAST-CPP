#ifndef ARGPARSE_HPP
#define ARGPARSE_HPP

#include <string>

enum class option : int {
    Query,
    GenerateECDF,
    Random,
    ComputeMatrix,
    MatrixSpecified,
    TimeSpecified,
    DefaultMode
};

std::string optToString(option m);

struct Args {
    int optind;
    std::string matrixFilepath, knownMatchesFilepath;
    option mode;
    uint64_t numRandomPairs;
    bool doSine = false;
    Args() : optind(0), 
             matrixFilepath(""),
             mode(option::DefaultMode),
             numRandomPairs(0) {}
    int parse(int argc, char *argv[]);
};

#endif // ARGPARSE_HPP
