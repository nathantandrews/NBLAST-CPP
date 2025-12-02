#ifndef ARGPARSE_HPP
#define ARGPARSE_HPP

#include <string>

constexpr const char *USAGE_MSG = 
"USAGE: ./nblast++ ... followed by one of the following:\n"
"    -q queryFile targetFile1 [targetFile2 ...]     # pair the query against all listed targets, produces .score files |\n"
"    -g swcFile1 [swcFile2 ...]                     # generate a p-value matrix for the swc files, prints a .matrix file to stdout |\n"
"    -n N swcFile2 [swcFile2 ...]                   # produce random pairs, ad infinitum if number of random pairs == -1, prints a .sin file to stdout |\n"
"    -s sinFile                                     # turn a sin file into a p-value matrix, produces a .matrix file |\n"
"    -r randomPairMatrixFile                        # read in the random pair matrix file\n"
"    -m matchPairMatrixFile                         # read in the match pair matrix file\n"
"    -c                                             # Calculate cosine angle measure instead of sine\n"
"    -h                                             # print usage message";
constexpr const char *INVALID_COMB_ERR_MSG = "invalid option combination: -%s and -%s\n";
constexpr const char *REQ_ARG_ERR_MSG = "option -%c requires an argument\n";
constexpr const char *INVALID_OPT_ERR_MSG = "invalid option: -%c\n";
constexpr const char *INVALID_ARG_ERR_MSG = "invalid argument: %s\n";
constexpr const char *OUT_OF_RANGE_ERR_MSG = "argument out of range: %s\n";
constexpr const char *FILEPATH_EMPTY_ERR_MSG = "%s filepath empty\n";
constexpr const char *FILE_OPENING_ERR_MSG = "cannot open file stream, file: %s";

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
void printUsage(std::ostream& out);
void invalidCombinationError(option m1, option m2);
void invalidArgumentError(const std::string& arg);
void outOfRangeError(const std::string& arg);
void requiredArgumentError(char option);
void invalidOptionError(char option);
void filepathEmptyError(const std::string& type);
void fileOpeningError(const std::string& type);

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
