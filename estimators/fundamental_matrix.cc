// Authors: Mihai-Dusmanu (mihaidusmanu), Paul-Edouard Sarlin (skydes)

#include "colmap/estimators/fundamental_matrix.h"

#include "colmap/math/random.h"
#include "colmap/optim/loransac.h"
#include "colmap/scene/camera.h"

#include <fstream>
#include <iostream>

using namespace colmap;

#include <pybind11/eigen.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;
using namespace pybind11::literals;

#include "log_exceptions.h"
#include "utils.h"

py::object fundamental_matrix_estimation(
    const std::vector<Eigen::Vector2d> points2D1,
    const std::vector<Eigen::Vector2d> points2D2,
    const RANSACOptions options) {
  SetPRNGSeed(0);
  THROW_CHECK_EQ(points2D1.size(), points2D2.size());
  py::object failure = py::none();
  py::gil_scoped_release release;

  LORANSAC<FundamentalMatrixSevenPointEstimator,
           FundamentalMatrixEightPointEstimator>
      ransac(options);
  const auto report = ransac.Estimate(points2D1, points2D2);
  if (!report.success) {
    return failure;
  }

  const Eigen::Matrix3d F = report.model;
  py::gil_scoped_acquire acquire;
  return py::dict("F"_a = F,
                  "num_inliers"_a = report.support.num_inliers,
                  "inliers"_a = ToPythonMask(report.inlier_mask));
}

void bind_fundamental_matrix_estimation(py::module& m) {
  auto est_options = m.attr("RANSACOptions")().cast<RANSACOptions>();

  m.def("fundamental_matrix_estimation",
        &fundamental_matrix_estimation,
        "points2D1"_a,
        "points2D2"_a,
        "estimation_options"_a = est_options,
        "LORANSAC + 7-point algorithm.");
}
