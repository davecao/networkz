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
 @brief CityBlock: a funcor to calculate city block distance for a given row-wised matrix.
 
 The observations are arranged as row variables.
 */
struct CityBlock
{
  /**
   @param mat A Eigen double matrix (m x n). m is the number of samples and n is the dimension of
             an observation..
   @return a Eign double matrix.
   */
  Eigen::MatrixXd operator()(const Eigen::MatrixXd& mat);
};

/*!
 @brief Euclidean: a funcor to calculate euclidean distance for a given row-wised matrix.
 
     The observations are arranged as row variables.
*/
struct Euclidean
{
  /**
   @param mat A Eigen double matrix (m x n). m is the number of samples and n is the dimension of
            an observation.
   @return a Eign double matrix.
   */
  Eigen::MatrixXd operator()(const Eigen::MatrixXd& mat);
};

/*!
 @brief Corrcoef: a functor to calculate pearson's correlation coefficient for a given row-wised matrix
 */
// Pearson's correlation coefficient
struct Corrcoef
{
  /**
   @param mat A Eigen double matrix (m x n). m is the number of samples and n is the dimension of
            an observation.
   @return a Eign double matrix.
   */
  Eigen::MatrixXd operator()(const Eigen::MatrixXd& mat);
};

/*!
 @brief create_graph: create a graph from a given dataframe defined in this project.
 @param g A pointer of a Graph
 @param df The dataframe data
 @param dist_threshold A threshold in double  to create an edge
        between two nodes if their distance be lower than this value.
 @param dist_functor A template argument for selecting the method to calculate the distance.
 */
// Create a graph from a given dataframe.
template<class DistType>
bool create_graph(Graph* g, DataFrame* df, double dist_threshold,
                  DistType dist_functor);

} // Namespace NARO

#endif /* graph_util_hpp */
