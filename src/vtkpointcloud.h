#pragma once

#include <Eigen/Dense>
#include <string>
#include <vector>

/*
 * Mini classe pour enregistrer le nuage de point au format VTK
 * pour visualiser dans ParaView.
 */

class vtkPointCloud {
public:
  vtkPointCloud(const std::vector<Eigen::Vector3d>& points);
  void save(const std::string& filename);

private:
  const std::vector<Eigen::Vector3d>& points;
};
