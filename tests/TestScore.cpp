#include "Test.hpp"
#include "Scoring.hpp"
#include "Matrix.hpp"
#include "MatrixIO.hpp"
#include "Point.hpp"

#include <iostream>

TEST_CASE(test_Scoring_basic) {
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

    Matrix mat = MatrixIO::loadMatrixFromTSV("tests/test_data/testLookUp.tsv");


    double score = scoreNeuronPair(mat, query, target, doCosine);

    // NaN check
    REQUIRE(score == score);

    // Optional: self-score
    double selfScore = scoreNeuronPair(mat, query, query, doCosine);

    REQUIRE(selfScore == selfScore);
}
