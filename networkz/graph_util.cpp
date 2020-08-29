//
//  graph_util.cpp
//  networkz
//
//  Created by CAO Wei on 2020/08/21.
//  Copyright © 2020 曹巍. All rights reserved.
//

#include "graph_util.hpp"

Eigen::MatrixXd NARO::CityBlock::operator()(const Eigen::MatrixXd& mat) {
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
Eigen::MatrixXd NARO::Euclidean::operator()(const Eigen::MatrixXd& mat) {
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
Eigen::MatrixXd NARO::Corrcoef::operator()(const Eigen::MatrixXd& mat) {
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
// The observations arranged as row variables.
template<class DistType>
bool NARO::create_graph(Graph* g, DataFrame* df,
                        double dist_threshold, DistType dist_functor) {
  bool result = false;
  
  NARO::NameVertexMap name2vertex;
  NARO::NameVertexMap::iterator pos_u;
  NARO::NameVertexMap::iterator pos_v;
  bool inserted;
  NARO::Vertex u, v;
  // Get data:
  auto dist_mat = dist_functor(df->data);
  size_t num_rows = dist_mat.rows();
  auto rownames = df->get_rowIndex_names();
  if(rownames.size() != num_rows) {
    std::cerr <<
      "The input dataframe has errors: string index != number of rows"
    << std::endl;
  }
#pragma omp parallel for
  for(int i=0; i<(num_rows - 1); i++) {
    int n1_inx = i;
    std::string n1 = rownames[n1_inx];
    double d_n1 = (*df)(n1_inx, 0);
    // Insert one node
    boost::tie(pos_u, inserted) = name2vertex.insert(
                                      std::make_pair(n1, NARO::Vertex()));
    if (inserted) {
      u = boost::add_vertex(NARO::gVertex{n1, d_n1}, *g);
      pos_u->second = u;
    } else {
      u = pos_u->second;
    }

    for(int j=i+1; j<num_rows; j++) {
      int n2_inx = j;
      std::string n2 = rownames[n2_inx];
      double d_n2 = (*df)(n2_inx, 0);
      // Create the other node in the graph
      boost::tie(pos_v, inserted) = name2vertex.insert(
                                    std::make_pair(n2, NARO::Vertex()));
      // Create an edge between these two nodes
      // if the distance between them be less than the dist_threshold
      if (inserted) {
        v = boost::add_vertex(NARO::gVertex{n2, d_n2}, *g);
        pos_v->second = v;
      } else {
        v = pos_v->second;
      }
      double d = dist_mat(i, j);
      if (d < dist_threshold ) {
        // Create an edge conecting those two vertices
        boost::add_edge(u, v, NARO::gEdge{d}, *g);
      }
    }
  }
  return result;
}
// -----------------------------------------------------------------------------
// Specialization of create_graph
// City block distance
// -----------------------------------------------------------------------------
template
bool NARO::create_graph<NARO::CityBlock>(Graph *g, DataFrame *df,
                                         double dist_threshold,
                                         NARO::CityBlock dist_functor);
// -----------------------------------------------------------------------------
// Specialization of create_graph
// Euclidean distance
// -----------------------------------------------------------------------------
template
bool NARO::create_graph<NARO::Euclidean>(Graph *g, DataFrame *df,
                                         double dist_threshold,
                                         NARO::Euclidean dist_functor);
// -----------------------------------------------------------------------------
// Specialization of create_graph
// Correlation coefficient
// -----------------------------------------------------------------------------
template
bool NARO::create_graph<NARO::Corrcoef>(Graph *g, DataFrame *df,
                                         double dist_threshold,
                                         NARO::Corrcoef dist_functor);

// -----------------------------------------------------------------------------
// Function
//
// -----------------------------------------------------------------------------
bool NARO::create_graph(Graph* g, DataFrame* df, double dist_threshold,
                        std::string distance_type)
{
  bool result = false;
  if (distance_type == "city") {
    NARO::CityBlock dist;
    result = NARO::create_graph(g, df,dist_threshold, dist);
  }else if (distance_type == "euc") {
    NARO::Euclidean dist;
    result = NARO::create_graph(g, df, dist_threshold, dist);
  } else if (distance_type == "corr"){
    NARO::Corrcoef dist;
    result = NARO::create_graph(g, df,dist_threshold, dist);
  } else {
    std::cerr << "Unknown method for distance computation" << std::endl;
  }
  return result;
}
