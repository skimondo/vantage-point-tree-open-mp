#include <fstream>
#include <iostream>
#include <benchmark/benchmark.h>
#include <cmath>    // for std::log2
#include <map>      // for storing optimal max_depth

#include "experiments.h"
#include "optparser.hpp"
#include "vpparallel.h"
#include "vpserial.cpp"

static const int NODE_COUNT = 100000;

void BM_VPTreeSerialBuild(benchmark::State& state) {

  std::vector<Vector3d> points;
  experiment_basic(points, NODE_COUNT);
  for (auto _ : state) {
    VPTreeSerial* tree = new VPTreeSerial(points);
    tree->build();
    delete tree;
  }
  state.SetComplexityN(NODE_COUNT);
}

void BM_VPTreeParallelBuild(benchmark::State& state) {

  std::vector<Vector3d> points;
  experiment_basic(points, NODE_COUNT);

  int max_depth = state.range(0);

  for (auto _ : state) {
    VPTreeParallel* tree = new VPTreeParallel(points);
    tree->setMaxDepth(max_depth);
    tree->build();
    delete tree;
  }
  state.SetComplexityN(NODE_COUNT);
}

int calculateMaxDepth(int node_count, int core_count) {
  // Use the smallest integer `depth` where 2^depth >= core_count
  int max_depth = static_cast<int>(std::ceil(std::log2(core_count)));
  // Ensure max_depth is non-negative
  if (max_depth < 0) max_depth = 0;
  max_depth += 2; // to allow more task in cas of cpu stall
  return max_depth;
}


void BM_VPTreeParallelWeakScaling(benchmark::State& state) {

  int core_count = state.range(0);
  int node_count = NODE_COUNT * core_count;
  int max_depth = calculateMaxDepth(node_count, core_count);

  std::vector<Vector3d> points;
  experiment_basic(points, node_count);

  for (auto _ : state) {
    VPTreeParallel* tree = new VPTreeParallel(points);
    tree->setMaxDepth(max_depth);

    auto start_time = std::chrono::high_resolution_clock::now();
    tree->build();
    auto end_time = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> build_duration = end_time - start_time;

    // Calculate throughput (nodes per second)
    double throughput = static_cast<double>(node_count) / build_duration.count();

    // Add custom counters to Google Benchmark's state
    state.counters["Nodes"] = node_count;
    state.counters["Cores"] = core_count;
    state.counters["Throughput"] = throughput; // Nodes per second
    state.counters["max_depth"] = max_depth;

    delete tree;
  }

  state.SetComplexityN(node_count);
}

BENCHMARK(BM_VPTreeSerialBuild);
BENCHMARK(BM_VPTreeParallelBuild)
    ->DenseRange(0, 25);

// Benchmark setup
BENCHMARK(BM_VPTreeParallelWeakScaling)
    ->DenseRange(1, 20)
    ->Complexity();

int main(int argc, char** argv) {
  benchmark::Initialize(&argc, argv);
  benchmark::RunSpecifiedBenchmarks();
  benchmark::Shutdown();
  return 0;
}
