#include "ArgParse.hpp"
#include "Point.hpp"
#include "Utils.hpp"
#include "FileIO.hpp"
#include "Error.hpp"
#include "nanoflann.hpp"
#include "LookUpTable.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <cstdint>
#include <cmath>
#include <limits>
#include <memory>
#include <iomanip>

// C-based includes
#include <unistd.h>
#include <cstring>
#include <cassert>

using DoubleVector = std::vector<double>;
using DoubleVector2D = std::vector<DoubleVector>;

PointVector buildMidpoints(const PointVector& pts) {
    PointVector mp;
    mp.reserve(pts.size());

    for (const auto& pt : pts) {
        if (pt.parent == -1) continue;
        Point m = pt.computeMidpoint(pts[pt.parent]);

        // midpoint: id = original id, parent = -1
        mp.emplace_back(pt.id, m.x, m.y, m.z, -1);
    }

    return mp;
}

PAVector nearestNeighborKDTree(const PointVector& query, const PointVector& target, bool doCosine, bool doPrint) {
    // Build midpoints for query / target
    PointVector queryMidpoints  = buildMidpoints(query);
    PointVector targetMidpoints = buildMidpoints(target);

    // Build point cloud for KD-tree
    PointCloud cloud(targetMidpoints);

    using KDTree = nanoflann::KDTreeSingleIndexAdaptor<
        nanoflann::L2_Simple_Adaptor<double, PointCloud>,
        PointCloud,
        3
    >;

    KDTree index(3, cloud, nanoflann::KDTreeSingleIndexAdaptorParams(10));
    index.buildIndex();

    PAVector matchVector;
    // For each query midpoint, perform nearest neighbor search
    for (const auto& qmp : queryMidpoints) {
        double query_pt[3] = { qmp.x, qmp.y, qmp.z };

        size_t nearestIdx = 0;
        double outDistanceSqr = 0;

        nanoflann::KNNResultSet<double> resultSet(1);
        resultSet.init(&nearestIdx, &outDistanceSqr);
        index.findNeighbors(resultSet, query_pt, 0);

        const Point& tmp = targetMidpoints[nearestIdx];

        // original query-side segment r_i
        const Point& qi = query.at(qmp.id);
        if (qi.parent == -1) continue;
        const Point& qj = query.at(qi.parent);
        Point r_i = qj - qi;

        // original target-side segment s_i
        const Point& ti = target.at(tmp.id);
        if (ti.parent == -1) continue;
        const Point& tj = target.at(ti.parent);
        Point s_i = tj - ti;

        // angle measure
        double angleMeasure = r_i.computeAngleMeasure(s_i, doCosine);

        // output: id_i id_j distance angle
        PointAlignment pc{ qi.id, ti.id, std::sqrt(outDistanceSqr), angleMeasure };
        matchVector[qi.id] = pc;
        if (doPrint) {
            pc.printDifference(std::cout);
        }
    }
    return matchVector;
}

PAVector nearestNeighborNaive(const PointVector& query, const PointVector& target, bool doCosine, bool doPrint) {
    PAVector matchVector;
    for (const auto& query_i : query) {
        if (query_i.parent == POINT_DEFAULT_PARENT) continue;
        
        const Point& query_j = query[query_i.parent];
        Point r_i = query_j - query_i;
        Point m_i = query_i + 0.5 * r_i;
        
        Point target_min;
        double distance_min = std::numeric_limits<double>::max();
        double angle_diff = 0;
        for (const auto& target_i : target) {
            if (target_i.parent == -1) continue;

            const Point& target_j = target[target_i.parent];
            Point s_i = target_j - target_i;
            Point o_i = target_i + 0.5 * s_i;

            double distance_i = m_i.computeDistance(o_i);
            
            if (distance_i < distance_min) {
                target_min = target_i;
                distance_min = distance_i;

                double angle_diff_tmp = r_i.computeAngleMeasure(s_i, doCosine);
                if (angle_diff_tmp == -1) continue;
                angle_diff = angle_diff_tmp;
            }
        }
        PointAlignment pm{ query_i.id, target_min.id, distance_min, angle_diff };
        matchVector[pm.queryPointID] = pm;
        if (doPrint) {
            pm.printDifference(std::cout);
        }
    }
    return matchVector;
}

void computeRawScores(const LookUpTable& lut, PAVector& matchVector) {
    for (auto& pm : matchVector) {
        pm.computeRawScore(lut);
    }
}

double sumRawScores(PAVector vec) {
    double res = 0;
    for (const auto& elem : vec) {
        res += elem.score;
    }
    return res;
}

double scoreNeuronPair(const LookUpTable& lut, 
                       const PointVector& queryVector, const PointVector& targetVector, bool doCosine) {
    // compute forward score
    PAVector forwardMatchVector = nearestNeighborKDTree(queryVector, targetVector, doCosine, false);
    computeRawScores(lut, forwardMatchVector);
    double forwardTotalScore = sumRawScores(forwardMatchVector);

    // compute forward self score
    PAVector forwardSelfMatchVector = nearestNeighborKDTree(queryVector, queryVector, doCosine, false);
    computeRawScores(lut, forwardSelfMatchVector);
    double forwardSelfTotalScore = sumRawScores(forwardSelfMatchVector);

    // compute reverse score
    PAVector reverseMatchVector = nearestNeighborKDTree(targetVector, queryVector, doCosine, false);
    computeRawScores(lut, reverseMatchVector);
    double reverseTotalScore = sumRawScores(reverseMatchVector);

    // compute reverse self score
    PAVector reverseSelfMatchVector = nearestNeighborKDTree(targetVector, targetVector, doCosine, false);
    computeRawScores(lut, reverseSelfMatchVector);
    double reverseSelfTotalScore = sumRawScores(reverseSelfMatchVector);
    
    // normalize forward and reverse by self
    // then average for final score
    return ((forwardTotalScore / forwardSelfTotalScore) + (reverseTotalScore / reverseSelfTotalScore)) / 2;
}

int main(int argc, char *argv[]) {
    Args a;
    a.parse(argc, argv);
    int rc;
    switch (a.mode) {
        case option::Query: {
            if (a.matrixFilepath.empty()) { 
                filepathEmptyError("Matrix");
            }

            LookUpTable lut;
            lut.loadFromTSV(a.matrixFilepath);
                        
            PointVector queryVector;
            a.optind = 3; // HARDCODED @todo fix
            std::string queryFilepath = argv[a.optind];
            std::cout << "queryFilepath: " << queryFilepath << std::endl;
            rc = loadPoints(queryFilepath, queryVector);
            assert(rc == 0);
            std::string strippedQuery = basenameNoExt(queryFilepath);

            for(int i = a.optind; i < argc; i++) {
                std::string targetFilepath = argv[i];
                std::cout << "targetFilepath: " << targetFilepath << std::endl;
                if (targetFilepath.empty()) { std::cerr << "can't open " << targetFilepath << "; continuing\n"; continue; }
                PointVector targetVector;
                rc = loadPoints(targetFilepath, targetVector);
                if (rc) { std::cerr << "failed to load points from " << targetFilepath << "; continuing\n"; continue; }
                std::string strippedTarget = basenameNoExt(targetFilepath);
                
                std::cerr << "scoring " << strippedQuery << " " << strippedTarget << "\n";
                double score = scoreNeuronPair(lut, queryVector, targetVector, a.doSine);
                std::cout << strippedQuery << " " << strippedTarget << " " << score << "\n";                
            }
            std::cout.flush();
            return 0;
        }
        // generate a matrix for a given dataset, print it to stdout
        case option::GenerateECDF: {
            // @todo finish mode
            return 0;
        }
        // first step in generating the matrix
        // randomly selects pairs of .swc files,
        // and outputs nearest neighbor calculations to stdout
        case option::Random: {
            uint64_t n = argc - optind; // number of input SWC files, from which random pairs will be chosen
            std::string queryFilepath, targetFilepath;
            srand48(time(0) + getpid()); // seed the random number generator
            while(a.numGeneratorIterations > 0) {
                uint64_t i = optind + n * drand48(), j = optind + n * drand48();

                queryFilepath = argv[i];
                PointVector queryVector;
                rc = loadPoints(queryFilepath, queryVector);
                if (rc == -1) continue;

                targetFilepath = argv[j];
                PointVector targetVector;
                rc = loadPoints(targetFilepath, targetVector);
                if (rc == -1) continue;

                std::cout << basenameNoExt(queryFilepath) << " " << basenameNoExt(targetFilepath) << "\n";
                PAVector matchVector = nearestNeighborKDTree(queryVector, targetVector, a.doSine, true);
            }
            return 0;
        }
        // in case something went wrong in argument parsing
        default: { invalidArgumentError(optToString(a.mode)); return -1; }
    }
}
