#include <benchmark/benchmark.h>

#include <cmath>  // for std::log2
#include <fstream>
#include <iostream>
#include <map>  // for storing optimal max_depth
#include <thread>

#include "experiments.h"
#include "optparser.hpp"
#include "vpparallel.h"
#include "vpserial.cpp"

static const int NODE_COUNT = 1000000;
static const int CORE_COUNT = std::thread::hardware_concurrency();
static int MAX_DEPTH_OF_TREE = static_cast<int>(std::floor(std::log2(NODE_COUNT)));

// Output file for benchmark data
std::ofstream granularite_file;
std::ofstream weak_scale_file;

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
    auto start_time = std::chrono::high_resolution_clock::now();
    tree->build();
    auto end_time = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> build_duration = end_time - start_time;

    if (granularite_file.is_open()) {
      granularite_file << max_depth << " " << build_duration.count() << "\n";
    }

    delete tree;
  }
  state.SetComplexityN(NODE_COUNT);
}

int calculateMaxDepth(int node_count, int core_count) {
  // Use the smallest integer `depth` where 2^depth >= core_count
  int max_depth = static_cast<int>(std::ceil(std::log2(core_count)));
  // Ensure max_depth is non-negative
  if (max_depth < 0)
    max_depth = 0;
  max_depth += 2;  // to allow more task in cas of cpu stall
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
    state.counters["Throughput"] = throughput;  // Nodes per second
    state.counters["max_depth"] = max_depth;

    if (weak_scale_file.is_open()) {
      weak_scale_file << core_count << " " << node_count << " " << build_duration.count() << " " << throughput << "\n";
    }

    delete tree;
  }

  state.SetComplexityN(node_count);
}

BENCHMARK(BM_VPTreeSerialBuild);
BENCHMARK(BM_VPTreeParallelBuild)->DenseRange(1, MAX_DEPTH_OF_TREE)->Iterations(10);

// Benchmark setup
BENCHMARK(BM_VPTreeParallelWeakScaling)->DenseRange(1, CORE_COUNT)->Complexity()->Iterations(10);

int main(int argc, char** argv) {
  granularite_file.open("granularite.dat");
  if (!granularite_file.is_open()) {
    std::cerr << "Failed to open granularite.dat for writing.\n";
    return 1;
  }
  weak_scale_file.open("weak_scale.dat");
  if (!granularite_file.is_open()) {
    std::cerr << "Failed to open weak_scale.dat for writing.\n";
    return 1;
  }

  benchmark::Initialize(&argc, argv);
  benchmark::RunSpecifiedBenchmarks();
  benchmark::Shutdown();

  granularite_file.close();
  weak_scale_file.close();
  return 0;
}
