#include "Error.hpp"

#include <iostream>
#include <string>

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
