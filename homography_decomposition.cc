#include <iostream>
#include <fstream>

#include "colmap/base/homography_matrix.h"

using namespace colmap;

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/eigen.h>

namespace py = pybind11;

py::dict homography_decomposition_estimation (
    const Eigen::Matrix3d H,
    const Eigen::Matrix3d K1,
    const Eigen::Matrix3d K2,
    const std::vector<Eigen::Vector2d> points1,
    const std::vector<Eigen::Vector2d> points2
) {
    SetPRNGSeed(0);

    // Check that both vectors have the same size.
    assert(points1.size() == points2.size());

    // Pointers to R, t, n
    Eigen::Matrix3d R;
    Eigen::Vector3d t;
    Eigen::Vector3d n;
    std::vector<Eigen::Vector3d> points3D;

    // Homography Decomposition Estimation
    PoseFromHomographyMatrix(H, K1, K2, points1, points2, &R, &t, &n, &points3D)

    // Success output dictionary.
    py::dict success_dict;
    success_dict["success"] = true;
    success_dict["R"] = R;
    success_dict["t"] = t;
    success_dict["n"] = n;
    
    return success_dict;
}