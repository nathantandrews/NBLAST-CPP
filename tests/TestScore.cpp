#include "Test.hpp"
#include "Scoring.hpp"
#include "LookUpTable.hpp"
#include "Point.hpp"

#include <iostream>

TEST_CASE(test_LookUpTable_basic) {
    bool doCosine = true;

    PointVector query = {
        Point(0, 0, 0, 0, -1),
        Point(1, 1, 0, 0, 0),
        Point(2, 2, 0, 0, 1),
        Point(3, 1, 1, 1, 1),
        Point(4, 2, 2, 0, 3)
    };

    PointVector target = {
        Point(0, 0, 1, 0, -1),
        Point(1, 0, 2, 0, 0),
        Point(2, 0, 3, 0, 1),
        Point(3, 1, 1, 0, 2),
        Point(4, 3, 3, 0, 2)
    };

    LookUpTable lut;
    REQUIRE(lut.loadFromTSV("tests/test_data/testLookUp.tsv"));


    double score = scoreNeuronPair(lut, query, target, doCosine);

    // NaN check
    REQUIRE(score == score);

    // Optional: self-score
    double selfScore = scoreNeuronPair(lut, query, query, doCosine);

    REQUIRE(selfScore == selfScore);
}
