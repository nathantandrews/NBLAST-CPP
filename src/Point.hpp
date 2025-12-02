#ifndef POINT_HPP
#define POINT_HPP

#include <iomanip>
#include <fstream>
#include <iostream>
#include <cmath>
#include <sstream>
#include <vector>

constexpr int POINT_DEFAULT_PARENT = -1;
constexpr int POINT_DEFAULT_ID = -1;

using DoubleVector = std::vector<double>;
using DoubleVector2D = std::vector<DoubleVector>;
using PointVector = std::vector<Point>;
using PMVector = std::vector<PointAlignment>;

// Individual points, stores parent, position, etc.
struct Point {
    int id, parent;
    double x, y, z;

    Point(int id, double x, double y, double z, int parent) 
        : id(id), x(x), y(y), z(z), parent(parent) {}
    Point() : id(POINT_DEFAULT_ID), x(0.0), y(0.0), z(0.0), 
        parent(POINT_DEFAULT_PARENT) {}
    
    void parse(std::string line);

    double magnitude(void) const;

    Point computeMidpoint(const Point& other) const {
        return *this + 0.5 * (other - *this);
    }

    double computeDistance(const Point& other) const {
        return (*this - other).magnitude();
    }

    double computeAngleMeasure(const Point& other, bool do_cosine) const;

    Point& operator=(const Point& other);

    friend double normedDotProduct(const Point& lhs, const Point& rhs);
    friend std::ostream& operator<<(std::ostream& out, const Point& p);
    friend Point operator-(const Point& lhs, const Point& rhs);
    friend Point operator+(const Point& lhs, const Point& rhs);
    friend Point operator*(const Point& lhs, double rhs);
    friend Point operator*(double lhs, const Point& rhs);
};

// Alignment structure, stores point-ids, distance, etc.
struct PointAlignment {
    int queryPointID, targetPointID;
    double distance, angleMeasure, score;

    PointAlignment(int queryPointID, int targetPointID, 
               double distance, double angleMeasure)
        : queryPointID(queryPointID), 
        targetPointID(targetPointID), 
        distance(distance), 
        angleMeasure(angleMeasure), 
        score(0) {}
    PointAlignment()
        : queryPointID(-1), 
        targetPointID(-1), 
        distance(0), 
        angleMeasure(0), 
        score(0) {}
    PointAlignment(const PointAlignment& other)
        : queryPointID(other.queryPointID),
        targetPointID(other.targetPointID),
        distance(other.distance),
        angleMeasure(other.angleMeasure),
        score(other.score) {}

    void computeRawScore(const DoubleVector2D& eCDFMatrix);

    void printDifference(std::ostream& out) const;

    inline void printScore(std::ostream& out) const;

    friend PointAlignment operator+(const PointAlignment& lhs, const PointAlignment& rhs);
};

// KD-tree cloud (const)
struct PointCloud
{
    const PointVector& pts;

    PointCloud(const PointVector& points) : pts(points) {}

    // nanoflann interface: number of points
    inline size_t kdtree_get_point_count() const { return pts.size(); }

    // nanoflann interface: coordinate for point index idx, dimension dim
    inline double kdtree_get_pt(size_t idx, size_t dim) const
    {
        const Point& p = pts[idx];
        if (dim == 0) return p.x;
        if (dim == 1) return p.y;
        return p.z;
    }

    // bounding-box (not used)
    template<class BBOX>
    bool kdtree_get_bbox(BBOX&) const { return false; }
};

#endif // POINT_HPP
