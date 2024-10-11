#include "experiments.h"

#include <random>

using namespace Eigen;

void experiment_basic(std::vector<Vector3d>& points, int n) {
  std::default_random_engine rng(0);
  std::uniform_real_distribution<double> r(0.0, 1.0);

  points.clear();
  points.reserve(n);
  for (int i = 0; i < n; i++) {
    points.push_back({r(rng), r(rng), r(rng)});
  }
}
