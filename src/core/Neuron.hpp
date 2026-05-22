#ifndef NEURON_HPP
#define NEURON_HPP

#include "Point.hpp"
#include "utils/nanoflann.hpp"
#include <memory>
#include <string>
#include <unordered_map>

typedef nanoflann::KDTreeSingleIndexAdaptor<
    nanoflann::L2_Simple_Adaptor<double, PointCloud>, PointCloud, 3>
    KDTree;

class Neuron {
public:
  Neuron(const std::string &filename);
  Neuron(Neuron &&) = default;
  Neuron(const Neuron &) = default;
  Neuron &operator=(Neuron &&) = default;
  Neuron &operator=(const Neuron &) = default;
  ~Neuron() = default;
  std::string getNID(void) const { return nid; }
  PAVector nearestNeighbors(const Neuron &target, bool doSine = false,
                            bool doPrint = false) const;
  double score(const Neuron &target, const Matrix &mat, bool doSine = false,
               bool doPrint = false) const;

private:
  std::string nid;
  std::vector<Point> pVector;
  std::vector<Point> mpVector;
  std::unordered_map<int, int> idToIndex;
  std::unique_ptr<PointCloud> cloud;
  std::unique_ptr<KDTree> index;
};

#endif // NEURON_HPP
