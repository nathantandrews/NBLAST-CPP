#include <iostream>
#include <vector>
#include <cassert>
#include <cmath>

#include "../src/Scoring.hpp"
#include "../src/LookUpTable.hpp"
#include "../src/Point.hpp"



int main()
{
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
    if (!lut.loadFromTSV("test/testLookUp.tsv")) {
        std::cerr << "ERROR: lookup table not found or failed to load\n";
        return 1;
    }

    std::cout << "Running scoring test...\n";

    double score = scoreNeuronPair(lut, query, target, doCosine);
    std::cout << "Score(query, target) = " << score << "\n";

    // NaN check
    if (score != score) {
        std::cerr << "ERROR: score is NaN\n";
        return 1;
    }

    // Optional: self-score
    double selfScore = scoreNeuronPair(lut, query, query, doCosine);
    std::cout << "Score(query, query) = " << selfScore << "\n";

    if (selfScore != selfScore) {
        std::cerr << "ERROR: selfScore is NaN\n";
        return 1;
    }

    std::cout << "Scoring test passed (finite results).\n";
    return 0;
}
