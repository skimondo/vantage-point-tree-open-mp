#pragma once

#include "oneapi/tbb/concurrent_vector.h"
#include "vpserial.h"

class VPTreeParallel : public INearestNeighbors {
public:
  VPTreeParallel(const std::vector<Eigen::Vector3d>& points);
  void build() override;
  SearchResults search(const Eigen::Vector3d& point, int n) const override;
  void setMaxDepth(int max_depth);
  typedef std::pair<Eigen::Vector3d, int> Item;

private:
  void makeItems();
  int makeTree(int lower, int upper, int depth, int m_max_depth);
  int makeNode(int item);
  void selectRoot(int lower, int upper);
  void partitionByDistance(int lower, int pos, int upper);
  void searchInNode(const VPNode& node,  //
                    SearchContext& ctx) const;

  int m_max_depth = std::numeric_limits<int>::max();  // default
  const std::vector<Eigen::Vector3d>& m_points;
  std::mt19937 rng_;
  tbb::concurrent_vector<VPNode> nodes_;
  std::vector<Item> items_;
};
