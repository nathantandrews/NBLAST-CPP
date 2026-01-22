#include "Point.hpp"
#include "Utils.hpp"

#include <vector>
#include <string>

using DoubleVector = std::vector<double>;
using DoubleVector2D = std::vector<DoubleVector>;

// ================= Point Definitions =================

void Point::parse(std::string line) {
    std::istringstream sin{line};
    std::string ignore;
    sin >> this->id 
        >> ignore 
        >> this->x >> this->y >> this->z 
        >> ignore 
        >> this->parent;
}
double Point::magnitude(void) const {
    return std::sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
}
Point Point::computeMidpoint(const Point& other) const {
    return *this + 0.5 * (other - *this);
}
double Point::computeDistance(const Point& other) const {
    return (*this - other).magnitude();
}
double Point::computeAngleMeasure(const Point& other, bool do_sine) const {
    double selfMagnitude = this->magnitude();
    double otherMagnitude = other.magnitude();
    if (selfMagnitude == 0 || otherMagnitude == 0) return -1;
    double angleMeasure = std::abs(normedDotProduct(*this, other) / (selfMagnitude * otherMagnitude));
    if(angleMeasure > 1) angleMeasure = 1;
    if (do_sine) return sin(acos(angleMeasure)); 
    else return angleMeasure;
}
Point& Point::operator=(const Point& other) {
    if (this != &other)
    {
        this->id = other.id;
        this->x = other.x;
        this->y = other.y;
        this->z = other.z;
        this->parent = other.parent;
    }
    return *this;
}

double normedDotProduct(const Point& lhs, const Point& rhs) {
    return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}
std::ostream& operator<<(std::ostream& out, const Point& p) {
    out << std::fixed << std::setprecision(4)
        << "id: '" << p.id << "' "
        << " x: '" << p.x << "' "
        << " y: '" << p.y << "' "
        << " z: '" << p.z << "' "
        << "parent: '" << p.parent << "'\n";
    return out;
}
Point operator-(const Point& lhs, const Point& rhs) {
    return Point(POINT_DEFAULT_ID, lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, POINT_DEFAULT_PARENT);
}
Point operator+(const Point& lhs, const Point& rhs) {
    return Point(POINT_DEFAULT_ID, lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, POINT_DEFAULT_PARENT);
}
Point operator*(const Point& lhs, double rhs) {
    return Point(lhs.id, lhs.x * rhs, lhs.y * rhs, lhs.z * rhs, lhs.parent);
}
Point operator*(double lhs, const Point& rhs) {
    return Point(rhs.id, lhs * rhs.x, lhs * rhs.y, lhs * rhs.z, rhs.parent);
}

// ================= PointAlignment Definitions =================
void PointAlignment::computeRawScore(const LookUpTable& lut) {
    if (distance < 0 || angleMeasure < 0) {
        std::cerr << "Invalid distance or angleMeasure\n";
        exit(EXIT_FAILURE);
    }
    this->score = lut.lookUp(distance, angleMeasure);
}
void PointAlignment::printDifference(std::ostream& out, const std::string& tag) const {
    if (tag.size()) {
       out << tag << ": ";
    }
    out << this->queryPointID << " " 
        << this->targetPointID << " " 
        << this->distance << " " 
        << this->angleMeasure << "\n";
}
void PointAlignment::printScore(std::ostream& out) const {
    out << this->queryPointID << " "
        << this->targetPointID << " "
        << this->score << "\n";
}
PointAlignment PointAlignment::operator=(const PointAlignment& other) {
    if (this != &other)
    {
        this->queryPointID = other.queryPointID;
        this->targetPointID = other.targetPointID;
        this->distance = other.distance;
        this->angleMeasure = other.angleMeasure;
        this->score = other.score;
    }
    return *this;
}

PointAlignment operator+(const PointAlignment& lhs, const PointAlignment& rhs) {
    PointAlignment pa = PointAlignment();
    pa.score = lhs.score + rhs.score;
    return pa;
}
