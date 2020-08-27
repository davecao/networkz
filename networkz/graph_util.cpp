//
//  graph_util.cpp
//  networkz
//
//  Created by CAO Wei on 2020/08/21.
//  Copyright © 2020 曹巍. All rights reserved.
//

#include "graph_util.hpp"

Eigen::MatrixXd NARO::city_block(const Eigen::MatrixXd& mat) {
  int nrows = static_cast<int>(mat.rows());
  Eigen::MatrixXd d_mat(nrows, nrows);
  std::cout << mat << std::endl;
  for (int rowIdx=0; rowIdx < nrows; rowIdx++) {
    d_mat.row(rowIdx) = (mat.rowwise() - mat.row(rowIdx)).matrix().rowwise().sum();
    std::cout << d_mat << std::endl;
  }
  return d_mat.cwiseAbs();
}

// -----------------------------------------------------------------------------
// Eigen uses Column Major definition of Matrix in general if not specified.
//
Eigen::MatrixXd NARO::euclidean(const Eigen::MatrixXd& mat) {
  auto d_mat = ((-2 * mat.transpose() * mat).colwise() +
                mat.colwise().squaredNorm().transpose()).rowwise() +
               mat.colwise().squaredNorm();
  return d_mat;
}
// -----------------------------------------------------------------------------
// Function corrcoef
// Pearson's correlation coefficient for real numbers.
//   mat - the observations are arranged as rows.
//
Eigen::MatrixXd NARO::corrcoef(const Eigen::MatrixXd& mat) {
  double nrows = static_cast<double>(mat.rows());
  if (nrows == 1) {
    std::cerr << "Error: the input matrix at least contains two rows"
              << std::endl;
  }
  // Centered the data matrix
  auto m = mat.rowwise().mean();
  Eigen::MatrixXd Xc = mat.colwise() - m;
  // Calculate the covariance matrix
  Eigen::MatrixXd cov = Xc * Xc.transpose() / (nrows - 1);
  // Calculate the correlation coefficient
  // By matrix:
  //Eigen::VectorXd diag = cov.diagonal().array().sqrt();
  //Eigen::MatrixXd Dinv = diag.asDiagonal().inverse();
  //Eigen::MatrixXd cc = Dinv * cov * Dinv;
  // By array:
  Eigen::VectorXd diag = cov.diagonal();
  Eigen::VectorXd D = diag.array().rsqrt();
  Eigen::MatrixXd cc = (cov.array().rowwise() * D.transpose().array()).colwise() * D.array();

//  std::cout << "Raw matrix: arranged as rows\n";
//  std::cout << mat << std::endl;
//  std::cout << "Mean vector: \n";
//  std::cout << m << std::endl;
//  std::cout << "Centered matrix:\n";
//  std::cout << Xc << std::endl;
//  std::cout << "Covariance matrix:\n";
//  std::cout << cov << std::endl;
//  std::cout << "diagnal element in row vector:\n";
//  std::cout << diag << std::endl;
//  std::cout << "Inverse of squared root of diagnal element matrix\n";
//  std::cout << D << std::endl;
//  std::cout << "correlation coefficient matrix for mat:\n";
//  std::cout << cc << std::endl;

  return cc;
}

// -----------------------------------------------------------------------------
//
bool create_graph_ct(NARO::Graph* g, NARO::DataFrame* df) {
  bool result = false;
  return result;
}
// -----------------------------------------------------------------------------
//
bool create_graph_corr(NARO::Graph* g, NARO::DataFrame* df)
{
  bool result = false;
  auto mat = df->data;
  // Centered the df
  Eigen::MatrixXd centered = mat.rowwise() - mat.colwise().mean();
  // Calculate pearson correlation coefficient
  Eigen::MatrixXd cov = (centered.adjoint() * centered) / double(mat.rows() - 1);
  // 
  return result;
}
// -----------------------------------------------------------------------------
//
bool NARO::create_graph(Graph* g, DataFrame* df, Distance d) {
  bool result = false;
  return result;
}
