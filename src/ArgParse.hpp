#ifndef ARGPARSE_HPP
#define ARGPARSE_HPP

#include <string>

enum class option_t : int {
    Query,
    GenerateECDF,
    Random,
    ComputeMatrix,
    MatrixSpecified,
    TimeSpecified,
    DefaultMode
};

std::string optToString(option_t m);

struct Args {
    int optind;
    std::string matrixFilepath, knownMatchesFilepath;
    option_t mode;
    uint64_t numGeneratorIterations;
    bool doSine = false;
    Args() : optind(0), 
             matrixFilepath(""),
             mode(option_t::DefaultMode),
             numGeneratorIterations(0) {}
    int parse(int argc, char *argv[]);
};

#endif // ARGPARSE_HPP
