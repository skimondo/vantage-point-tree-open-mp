#include <uqam/tp.h>

#include <Eigen/Dense>
#include <filesystem>
#include <format>
#include <iostream>
#include <limits>
#include <vector>

#include "experiments.h"
#include "optparser.hpp"
#include "vpparallel.h"
#include "vpserial.h"
#include "vtkpointcloud.h"

using namespace Eigen;

int main(int argc, char** argv) {
  int parallel = 0;
  int num_points = 10;
  int max_depth = std::numeric_limits<int>::max();

  OptionsParser args(argc, argv);
  args.AddOption(&num_points, "-n", "--num-points", "number of points");
  args.AddOption(&parallel, "-p", "--parallel", "parallel engine (0: serial, 1: tbb)");
  args.AddOption(&max_depth, "-d", "--depth", "max depth (grain size) (default: MAX_INT)");

  args.Parse();
  if (!args.Good()) {
    args.PrintUsage(std::cout);
    return 1;
  }
  args.PrintOptions(std::cout);

  // Générer des points
  std::vector<Vector3d> points;
  experiment_basic(points, num_points);

  // Sauvegarder le nuage de point
  {
    vtkPointCloud vpc(points);
    vpc.save("points.vtk");
  }

  // Construire l'arbre VP
  INearestNeighbors* tree;
  if (parallel == 0) {
    tree = new VPTreeSerial(points);
  } else {
    // un peu hack pour set max depth, mais evite de changer la signature
    VPTreeParallel* parallelTree = new VPTreeParallel(points);
    parallelTree->setMaxDepth(max_depth);
    tree = parallelTree;
  }

  std::cout << "construction de l'arbre... " << std::flush;
  tree->build();
  std::cout << "fait" << std::endl;

  // Trouver les 10 voisins les plus proches
  {
    std::cout << "calcul des voisins... " << std::flush;
    SearchResults res = tree->search({0.5, 0.5, 0.5}, 10);
    std::cout << "fait" << std::endl;

    for (int i = 0; i < res.size(); i++) {
      std::printf("%04d %09d %0.8f\n", i, res[i].idx, res[i].dist);
    }
  }

  delete tree;
  std::cout << "Fin normale du programme" << std::endl;
  return 0;
}
