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

enum class Distance: int
{
  city_block,
  euclidean,
  correlation
};

// Distance
// City block distance
Eigen::MatrixXd city_block(const Eigen::MatrixXd& mat);

// Euclidean distance
Eigen::MatrixXd euclidean(const Eigen::MatrixXd& mat);

// Pearson's correlation coefficient
Eigen::MatrixXd corrcoef(const Eigen::MatrixXd& mat);

// Create a graph from a given dataframe.
bool create_graph(Graph* g, DataFrame* df, Distance d);

} // Namespace NARO

#endif /* graph_util_hpp */
