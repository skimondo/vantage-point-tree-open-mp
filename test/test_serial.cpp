#include <experiments.h>
#include <vpserial.h>

#include <Eigen/Dense>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include <vector>

#include "testutils.h"

using namespace Eigen;

TEST_CASE("VPTreeSerialSimple") {
  auto points = std::vector<Eigen::Vector3d>{{0, 0, 1}, {1, 1, 1}, {2, 0, 0}, {-1, -1, 0}, {10, 0, 5}};

  VPTreeSerial t(points);
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

TEST_CASE("VPTreeSerialSmall") {
  std::vector<Vector3d> points;
  experiment_basic(points, 10);

  auto tree = VPTreeSerial(points);
  auto naive = NaiveNearestNeighbors(points);

  // On construire l'arbre avec build()
  tree.build();

  for (const auto& p : points) {
    const auto neighborsCount = 3;
    SearchResults exp = naive.search(p, neighborsCount);
    SearchResults act = tree.search(p, neighborsCount);

    REQUIRE_THAT(exp, Catch::Matchers::Equals(act));
  }
}

TEST_CASE("VPTreeSerialLarge") {
  std::vector<Vector3d> points;
  experiment_basic(points, 1000);

  auto tree = VPTreeSerial(points);
  auto naive = NaiveNearestNeighbors(points);
  tree.build();

  for (const auto& p : points) {
    const auto neighborsCount = 10;
    SearchResults exp = naive.search(p, neighborsCount);
    SearchResults act = tree.search(p, neighborsCount);
    REQUIRE_THAT(exp, Catch::Matchers::Equals(act));
  }
}
