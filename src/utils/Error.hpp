#ifndef ERROR_HPP
#define ERROR_HPP

#include <string>

constexpr const char *USAGE_MSG =
    "USAGE: ./nblast++ ... followed by one of the following:\n"
    "    -q queryFile targetFile1 [targetFile2 ...]     # pair the query "
    "against all listed targets, produces .score files |\n"
    "    -g swcFile1 [swcFile2 ...]                     # generate a p-value "
    "matrix for the swc files, prints a .matrix file to stdout |\n"
    "    -c                                             # Calculate cosine "
    "angle measure instead of sine\n"
    "    -n                                             # Turn off bilinear "
    "interpolation in Matrix::score\n"
    "    -h                                             # print usage "
    "message\n";
constexpr const char *INVALID_COMB_ERR_MSG =
    "invalid option combination: -%s and -%s\n";
constexpr const char *REQ_ARG_ERR_MSG = "option -%c requires an argument\n";
constexpr const char *INVALID_OPT_ERR_MSG = "invalid option: -%c\n";
constexpr const char *INVALID_ARG_ERR_MSG = "invalid argument: %s\n";
constexpr const char *OUT_OF_RANGE_ERR_MSG = "argument out of range: %s\n";
constexpr const char *FILEPATH_EMPTY_ERR_MSG = "%s filepath empty\n";
constexpr const char *FILE_OPENING_ERR_MSG =
    "cannot open file stream, file: %s";

void printUsage(std::ostream &out);
void invalidCombinationError(const std::string &m1, const std::string &m2);
void invalidArgumentError(const std::string &arg);
void outOfRangeError(const std::string &arg);
void requiredArgumentError(char option);
void invalidOptionError(char option);
void filepathEmptyError(const std::string &type);
void fileOpeningError(const std::string &type);

#endif // ERROR_HPP
