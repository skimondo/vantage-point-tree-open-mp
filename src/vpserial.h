#pragma once

#include <Eigen/Dense>
#include <queue>
#include <random>
#include <vector>

struct SearchItem {
  SearchItem(double dist_, int idx_) : dist(dist_), idx(idx_) {
  }

  inline bool operator<(const SearchItem& rhs) const {
    // avec des distances uniques, on pourrait comparer seulement dist mais on utilise aussi l'indice dans la
    // comparaison pour obtenir un ordre total dans le cas de distances identiques.
    return std::tie(dist, idx) < std::tie(rhs.dist, idx);
  }

  inline bool operator==(const SearchItem& rhs) const {
    // ici on se permet d'utiliser la comparaison de double avec l'égalité, considérant que le calcul de la distance
    // n'est pas sujet à variation.
    return std::tie(dist, idx) == std::tie(rhs.dist, rhs.idx);
  }

  double dist;
  int idx;
};

typedef std::priority_queue<SearchItem> Prio;
typedef std::vector<SearchItem> SearchResults;

struct VPNode {
  static const int Leaf = -1;

  VPNode(int item, double threshold = 0., int left = Leaf, int right = Leaf)
      : item(item), threshold(threshold), left(left), right(right) {
  }

  int item;
  double threshold;
  int left;
  int right;
};

class SearchContext {
public:
  SearchContext(const Eigen::Vector3d& target, int nb)
      : m_target(target), m_nb(nb), m_tau(std::numeric_limits<double>::max()) {
  }
  const Eigen::Vector3d& m_target;
  int m_nb;
  double m_tau;
  Prio m_heap;
};

class INearestNeighbors {
public:
  virtual ~INearestNeighbors() = default;
  virtual void build() = 0;
  virtual SearchResults search(const Eigen::Vector3d& point, int nb) const = 0;
};

class NaiveNearestNeighbors : public INearestNeighbors {
public:
  NaiveNearestNeighbors(const std::vector<Eigen::Vector3d>& points);
  void build() override;
  SearchResults search(const Eigen::Vector3d& target, int nb) const override;

private:
  const std::vector<Eigen::Vector3d>& m_points;
};

class VPTreeSerial : public INearestNeighbors {
public:
  VPTreeSerial(const std::vector<Eigen::Vector3d>& points);
  void build() override;
  SearchResults search(const Eigen::Vector3d& point, int n) const override;

  typedef std::pair<Eigen::Vector3d, int> Item;

private:
  void makeItems();
  int makeTree(int lower, int upper);
  int makeNode(int item);
  void selectRoot(int lower, int upper);
  void partitionByDistance(int lower, int pos, int upper);
  void searchInNode(const VPNode& node,  //
                    SearchContext& ctx) const;

  bool is_ready;
  const std::vector<Eigen::Vector3d>& m_points;
  std::mt19937 rng_;  // nécessaire pour la construction de l'arbre
  std::vector<VPNode> nodes_;
  std::vector<Item> items_;
};
