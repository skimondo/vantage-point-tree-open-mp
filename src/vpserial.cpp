#include "vpserial.h"

#include <Eigen/Dense>
#include <queue>

/*
 * Basé sur https://github.com/pderkowski/vptree
 */

using namespace Eigen;

NaiveNearestNeighbors::NaiveNearestNeighbors(const std::vector<Eigen::Vector3d>& points) : m_points(points) {
}

void NaiveNearestNeighbors::build() {
  // rien à faire
}

SearchResults NaiveNearestNeighbors::search(const Eigen::Vector3d& target, int nb) const {
  // Définition d'une file à priorité
  // On calcule la distance et on ajoute l'item dans la file à priorité
  // Les points les plus proches seront au début de la file
  Prio heap;
  double tau = std::numeric_limits<double>::max();

  for (int i = 0; i < m_points.size(); ++i) {
    double dist = (m_points[i] - target).norm();
    if (dist < tau) {
      if (heap.size() == nb) {
        heap.pop();
      }
      heap.push({dist, i});
      if (heap.size() == nb) {
        tau = heap.top().dist;
      }
    }
  }

  SearchResults results;
  results.reserve(nb);
  while (nb--) {
    auto& top = heap.top();
    results.push_back(SearchItem(top.dist, top.idx));
    heap.pop();
  }
  std::reverse(results.begin(), results.end());
  return results;
}

/*
 * VPTreeSerial
 */

VPTreeSerial::VPTreeSerial(const std::vector<Eigen::Vector3d>& points) : m_points(points), is_ready(false) {
}

void VPTreeSerial::build() {
  makeItems();

  std::random_device rd;
  // déterministe
  rng_.seed(0);
  nodes_.reserve(items_.size());
  makeTree(0, items_.size());
  is_ready = true;
}

void VPTreeSerial::makeItems() {
  items_.clear();
  items_.reserve(m_points.size());
  for (int i = 0; i < m_points.size(); i++) {
    items_.push_back(std::make_pair(m_points[i], i));
  }
}

int VPTreeSerial::makeTree(int lower, int upper) {
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
    n.left = makeTree(lower + 1, median);
    n.right = makeTree(median, upper);
    return node;
  }
}

int VPTreeSerial::makeNode(int item) {
  nodes_.push_back(VPNode(item));
  return nodes_.size() - 1;
}

void VPTreeSerial::selectRoot(int lower, int upper) {
  std::uniform_int_distribution<int> uni(lower, upper - 1);
  int root = uni(rng_);
  std::swap(items_[lower], items_[root]);
}

void VPTreeSerial::partitionByDistance(int lower, int pos, int upper) {
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

SearchResults VPTreeSerial::search(const Vector3d& target, int n) const {
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

void VPTreeSerial::searchInNode(const VPNode& node,  //
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
