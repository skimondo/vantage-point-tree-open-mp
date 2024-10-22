#include "vpparallel.h"

#include <Eigen/Dense>
#include <tbb/task_group.h>

/*
 * IDENTIFICATION : indiquer le ou les codes permanents des auteurs ici.
 *
 *
 */

using namespace Eigen;

VPTreeParallel::VPTreeParallel(const std::vector<Eigen::Vector3d>& points) : m_points(points) {
}

void VPTreeParallel::build() {
  makeItems();

  std::random_device rd;
  // déterministe
  rng_.seed(0);
  nodes_.reserve(items_.size());
  makeTree(0, items_.size(), 0, m_max_depth);
}

void VPTreeParallel::makeItems() {
  items_.clear();
  items_.reserve(m_points.size());
  for (int i = 0; i < m_points.size(); i++) {
    items_.push_back(std::make_pair(m_points[i], i));
  }
}

int VPTreeParallel::makeTree(int lower, int upper, int depth, int max_depth) {
  if (lower >= upper) {
    return VPNode::Leaf;
  } else if (lower + 1 == upper) {
    return makeNode(lower);
  } else {
    selectRoot(lower, upper);
    int median = (upper + lower) / 2;
    partitionByDistance(lower, median, upper);
    auto node = makeNode(lower);
    auto& n = nodes_[node];
    n.threshold = (items_[lower].first - items_[median].first).norm();

    if (depth < max_depth) {
      tbb::task_group g;
      g.run([&] {
        n.left = makeTree(lower + 1, median, depth + 1, max_depth);
      });
      g.run([&] {
        n.right = makeTree(median, upper, depth + 1, max_depth);
      });
      g.wait();
    } else {
      n.left = makeTree(lower + 1, median, depth + 1, max_depth);
      n.right = makeTree(median, upper, depth + 1, max_depth);
    }
    return node;
  }
}

int VPTreeParallel::makeNode(int item) {
  auto it = nodes_.push_back(VPNode(item));
  return static_cast<int>(std::distance(nodes_.begin(), it));
}

void VPTreeParallel::selectRoot(int lower, int upper) {
  std::uniform_int_distribution<int> uni(lower, upper - 1);
  int root = uni(rng_);
  std::swap(items_[lower], items_[root]);
}

void VPTreeParallel::partitionByDistance(int lower, int pos, int upper) {
  std::nth_element(                //
      items_.begin() + lower + 1,  //
      items_.begin() + pos,        //
      items_.begin() + upper,      //
      [lower, this](const Item& i1, const Item& i2) {
        double d1 = (items_[lower].first - i1.first).norm();
        double d2 = (items_[lower].first - i2.first).norm();
        return d1 < d2;
      });
}

SearchResults VPTreeParallel::search(const Vector3d& target, int n) const {
  SearchContext ctx(target, n);
  const VPNode& root = nodes_[0];
  searchInNode(root, ctx);

  SearchResults results;
  while (!ctx.m_heap.empty()) {
    auto& item = ctx.m_heap.top();
    results.push_back(SearchItem(item.dist, items_[item.idx].second));
    ctx.m_heap.pop();
  }
  std::reverse(results.begin(), results.end());
  return results;
}

void VPTreeParallel::searchInNode(const VPNode& node,  //
                                  SearchContext& ctx) const {
  int item = node.item;
  const Item& tmp = items_[item];
  const Vector3d& coord = tmp.first;
  double dist = (coord - ctx.m_target).norm();

  if (dist < ctx.m_tau) {
    if (ctx.m_heap.size() == ctx.m_nb) {
      ctx.m_heap.pop();
    }

    ctx.m_heap.push({dist, node.item});

    if (ctx.m_heap.size() == ctx.m_nb) {
      ctx.m_tau = ctx.m_heap.top().dist;
    }
  }

  if (dist < node.threshold) {
    if (node.left != VPNode::Leaf && dist - ctx.m_tau <= node.threshold) {
      searchInNode(nodes_[node.left], ctx);
    }

    if (node.right != VPNode::Leaf && dist + ctx.m_tau >= node.threshold) {
      searchInNode(nodes_[node.right], ctx);
    }
  } else {
    if (node.right != VPNode::Leaf && dist + ctx.m_tau >= node.threshold) {
      searchInNode(nodes_[node.right], ctx);
    }

    if (node.left != VPNode::Leaf && dist - ctx.m_tau <= node.threshold) {
      searchInNode(nodes_[node.left], ctx);
    }
  }
}

void VPTreeParallel::setMaxDepth(int max_depth) {
  m_max_depth = max_depth;
}


