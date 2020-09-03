//
//  graph_util.hpp
//  networkz
//
//  Created by CAO Wei on 2020/08/21.
//  Copyright © 2020 曹巍. All rights reserved.
//

#ifndef graph_util_hpp
#define graph_util_hpp

#include <stdio.h>
#include <iostream>

#include <Eigen/Dense>

#include "dataframe.hpp"
#include "graph.hpp"

namespace NARO {

/**
 * @brief CityBlock: a funcor to calculate city block distance for a given row-wised matrix.
 *
 * The observations are arranged as row variables.
 */
struct CityBlock
{
  /**
   * @param[in] mat A Eigen double matrix (m x n). m is the number of samples and n is the dimension of
   *          an observation..
   * @returns a Eign double matrix.
   */
  Eigen::MatrixXd operator()(const Eigen::MatrixXd& mat);
};

/**
 * @brief Euclidean: a funcor to calculate euclidean distance for a given row-wised matrix.
 *
 * The observations are arranged as row variables.
 */
struct Euclidean
{
  /**
   * @param[in] mat A Eigen double matrix (m x n). m is the number of samples and n is the dimension of
   *         an observation.
   * @return a Eign double matrix.
   */
  Eigen::MatrixXd operator()(const Eigen::MatrixXd& mat);
};

/**
 * @brief Corrcoef: a functor to calculate pearson's correlation coefficient for a given row-wised matrix
 */
struct Corrcoef
{
  /**
   * @param[in] mat A Eigen double matrix (m x n). m is the number of samples and n is the dimension of
   *         an observation.
   * @return a Eign double matrix.
   */
  Eigen::MatrixXd operator()(const Eigen::MatrixXd& mat);
};
/**
 * @brief create_graph: create a graph from a given data
 *
 * @param[out] g A pointer of a Graph
 * @param[in] df The dataframe data
 * @param[in] dist_threshold A threshold in double  to create an edge
 *       between two nodes if their distance be lower than this value.
 * @param[in] distance_type the method name for distance computation. ['city', 'euc', 'corr'].
 */
bool create_graph(Graph* g,
                  DataFrame* df,
                  double dist_threshold,
                  std::string distance_type);
/**
 * @brief create_graph: create a graph from a given dataframe defined in this project.
 *
 * @param[out] g A pointer of a Graph
 * @param[in] df The dataframe data
 * @param[in] dist_threshold A threshold in double  to create an edge
 *       between two nodes if their distance be lower than this value.
 * @param[in] dist_functor A template argument for selecting the method to calculate the distance.
 */
template<class DistType>
bool create_graph(Graph* g,
                  DataFrame* df,
                  double dist_threshold,
                  DistType dist_functor);
/**
 * @brief A writer for graphviz format (dot file)
 *
 * Write to a graphviz file for a given graph
 *
 * @param[in] filename the path of the output file
 * @param[in] g A pointer of a Graph
 * @returns bool, true for success; false for failure
 */
bool write_to_graphviz(std::string& filename, Graph& g);

} // Namespace NARO

#endif /* graph_util_hpp */
