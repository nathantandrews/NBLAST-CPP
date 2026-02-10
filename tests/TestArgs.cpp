#include "Test.hpp"
#include "ArgParse.hpp"

#include <vector>
#include <unistd.h>
#include <sys/wait.h>

extern int optind;

// ==================== helpers ====================
static std::vector<char*> make_argv(std::vector<const char*> args) {
    std::vector<char*> result;
    for (auto s : args)
        result.push_back(const_cast<char*>(s));
    result.push_back(nullptr);
    return result;
}

// ==================== tests ====================

TEST_CASE(test_optToString_query) {
    REQUIRE_EQ(optToString(option_t::Query), "q");
    REQUIRE_EQ(optToString(option_t::GenerateECDF), "g");
    REQUIRE_EQ(optToString(option_t::MatrixSpecified), "m");
    REQUIRE_EQ(optToString(option_t::DefaultMode), "default");
}

TEST_CASE(test_parseFilePair_valid) {
    auto [file, iters] = parseFilePair("matches.tsv,42");

    REQUIRE_EQ(file, "matches.tsv");
    REQUIRE_EQ(iters, 42u);
}

TEST_CASE(test_parseFilePair_large_number) {
    auto [file, iters] = parseFilePair("foo,1000000");

    REQUIRE_EQ(file, "foo");
    REQUIRE_EQ(iters, 1'000'000u);
}

TEST_CASE(test_args_parse_query_mode) {
    optind = 1;
    Args args;

    auto argv = make_argv({
        "prog",
        "-q",
        "matrix.tsv"
    });

    int argc = argv.size() - 1;

    args.parse(argc, argv.data());

    REQUIRE_EQ(args.mode, option_t::Query);
    REQUIRE_EQ(args.matrixFilepath, "matrix.tsv");
}

TEST_CASE(test_args_parse_generate_mode) {
    optind = 1;
    Args args;

    auto argv = make_argv({
        "prog",
        "-g",
        "known.tsv,123"
    });

    int argc = argv.size() - 1;

    args.parse(argc, argv.data());

    REQUIRE_EQ(args.mode, option_t::GenerateECDF);
    REQUIRE_EQ(args.knownMatchesFilepath, "known.tsv");
    REQUIRE_EQ(args.numGeneratorIterations, 123u);
}

TEST_CASE(test_args_parse_sine_flag) {
    optind = 1;
    Args args;

    auto argv = make_argv({
        "prog",
        "-s",
        "-q",
        "matrix.swc"
    });

    int argc = argv.size() - 1;

    args.parse(argc, argv.data());

    REQUIRE(args.doSine);
}
