#ifndef ARGPARSE_HPP
#define ARGPARSE_HPP

#include <string>

enum class option_t : int {
    Query,
    GenerateScoringMatrix,
    Random,
    ComputeMatrix,
    MatrixSpecified,
    TimeSpecified,
    InputDirectoriesSpecified,
    DefaultMode
};
std::ostream& operator<<(std::ostream& out, option_t op);
std::string optToString(option_t m);

struct Args {
    int optind = 0;
    std::string matrixFilepath;
    std::string knownMatchesFilepath;
    std::string queryDatasetFilepath;
    std::string targetDatasetFilepath;
    option_t mode = option_t::DefaultMode;
    uint64_t numGeneratorIterations = 0;
    bool doSine = false;

    friend std::ostream& operator<<(std::ostream& out, const Args& a);
    int parse(int argc, char *argv[]);
private:
    void setMode(option_t next);
};

#endif // ARGPARSE_HPP
