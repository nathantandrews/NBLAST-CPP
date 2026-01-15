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
    uint64_t numGeneratorIterations;
    bool doSine = false;
    Args() : optind(0), 
             matrixFilepath(""),
             mode(option::DefaultMode),
             numGeneratorIterations(0) {}
    int parse(int argc, char *argv[]);
};

#endif // ARGPARSE_HPP
