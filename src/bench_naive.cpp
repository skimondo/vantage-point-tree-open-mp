#include <benchmark/benchmark.h>

#include "vpserial.cpp"
#include "experiments.h"

#include <random>

void BM_NaiveNearestNeighbors(benchmark::State& state) {
  std::vector<Vector3d> points;
  experiment_basic(points, state.range(0));

  std::default_random_engine rng(0);
  std::uniform_real_distribution<double> dist(0.0, 1.0);

  NaiveNearestNeighbors nn(points);
  for (auto _ : state) {
    Vector3d target{dist(rng), dist(rng), dist(rng)};
    nn.search(target, 10);
  }
  state.SetComplexityN(state.range(0));
}
BENCHMARK(BM_NaiveNearestNeighbors)
    ->RangeMultiplier(2)
    ->Range((1 << 4), (1 << 20))
    ->Complexity();

int main(int argc, char** argv) {
  benchmark::Initialize(&argc, argv);
  benchmark::RunSpecifiedBenchmarks();
  benchmark::Shutdown();
  return 0;
}
