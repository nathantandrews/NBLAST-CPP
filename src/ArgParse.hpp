#ifndef ARGPARSE_HPP
#define ARGPARSE_HPP

#include <string>
#include <vector>

enum class option_t : int {
    Query,
    GenerateScoringMatrix,
    Random,
    ComputeMatrix,
    MatrixSpecified,
    TimeSpecified,
    InputDirectoriesSpecified,
    DumpIntermediarySteps,
    DefaultMode
};
std::ostream& operator<<(std::ostream& out, option_t op);
std::string optToString(option_t m);

using StringVector = std::vector<std::string>;
struct Args {
    StringVector positionalArgs;
    std::string matrixFilepath;
    std::string knownMatchesFilepath;
    std::string queryDatasetFilepath;
    std::string targetDatasetFilepath;
    std::string matrixOutfile;
    option_t mode = option_t::DefaultMode;
    uint64_t numGeneratorIterations = 0;
    bool doSine = false;
    bool doDump = false;

    friend std::ostream& operator<<(std::ostream& out, const Args& a);
};

Args parseArgs(int argc, char *argv[]);

#endif // ARGPARSE_HPP
