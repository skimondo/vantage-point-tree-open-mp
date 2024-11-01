#include <experiments.h>
#include <uqam/tp.h>
#include <vpparallel.h>
#include <vpserial.h>

#include <Eigen/Dense>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/matchers/catch_matchers_predicate.hpp>
#include <catch2/matchers/catch_matchers_quantifiers.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>

#include "testutils.h"

#define DATA_DIR SOURCE_DIR "/test/data"

using namespace Catch;
using namespace Eigen;

TEST_CASE("VPTreeParallelSimple") {
  auto points = std::vector<Eigen::Vector3d>{{0, 0, 1}, {1, 1, 1}, {2, 0, 0}, {-1, -1, 0}, {10, 0, 5}};

  VPTreeParallel t(points);
  t.build();

  SearchResults results = t.search({0, 0, 0}, 3);
  REQUIRE(results.size() == 3);

  REQUIRE(results[0].idx == 0);
  REQUIRE(results[0].dist == 1);

  REQUIRE(results[1].idx == 3);
  REQUIRE(results[1].dist == std::sqrt(2));

  REQUIRE(results[2].idx == 1);
  REQUIRE(results[2].dist == std::sqrt(3));
}

TEST_CASE("VPTreeParallelSmall") {
  std::vector<Vector3d> points;
  experiment_basic(points, 10);

  VPTreeParallel tree(points);
  NaiveNearestNeighbors naive(points);

  tree.build();

  for (const auto& p : points) {
    const auto neighborsCount = 3;
    SearchResults act = tree.search(p, neighborsCount);
    SearchResults exp = naive.search(p, neighborsCount);

    REQUIRE_THAT(exp, Catch::Matchers::Equals(act));
  }
}

TEST_CASE("VPTreeParallelLarge") {
  std::vector<Vector3d> points;
  experiment_basic(points, 1000);

  VPTreeParallel tree(points);
  NaiveNearestNeighbors naive(points);

  tree.build();

  for (const auto& p : points) {
    const auto neighborsCount = 10;
    SearchResults act = tree.search(p, neighborsCount);
    SearchResults exp = naive.search(p, neighborsCount);

    REQUIRE_THAT(exp, Catch::Matchers::Equals(act));
  }
}

TEST_CASE("VPTreeParallelVLarge") {
  std::vector<Vector3d> points;
  experiment_basic(points, 10000);

  VPTreeParallel tree(points);
  NaiveNearestNeighbors naive(points);

  tree.build();

  for (const auto& p : points) {
    const auto neighborsCount = 100;
    SearchResults act = tree.search(p, neighborsCount);
    SearchResults exp = naive.search(p, neighborsCount);

    REQUIRE_THAT(exp, Catch::Matchers::Equals(act));
  }
}
