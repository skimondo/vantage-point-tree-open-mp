#include "vtkpointcloud.h"

#include <Eigen/Dense>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

void vtkPointCloud::save(const std::string& filename) {
  std::ofstream out(filename);
  if (!out.is_open()) {
    std::cerr << "Error: Could not open the file for writing.\n";
    return;
  }

  // Writing the VTK header
  out << "# vtk DataFile Version 3.0\n";
  out << "Point cloud data\n";
  out << "ASCII\n";
  out << "DATASET POLYDATA\n";

  // Writing the points
  out << "POINTS " << points.size() << " float\n";
  for (const auto& point : points) {
    out << point[0] << " " << point[1] << " " << point[2] << "\n";
  }

  // Writing the vertices
  int nv = points.size() * 2;
  out << "VERTICES " << points.size() << " " << nv << "\n";
  for (int i = 0; i < points.size(); i++) {
    out << 1 << " " << i << "\n";
  }

  out.close();
}

vtkPointCloud::vtkPointCloud(const std::vector<Eigen::Vector3d>& points) : points(points) {
}
