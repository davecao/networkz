//
//  graph_algo.hpp
//  networkz
//
//  Created by 曹巍 on 2020/08/30.
//  Copyright © 2020 曹巍. All rights reserved.
//

#ifndef graph_algo_hpp
#define graph_algo_hpp

#include <iostream>
#include <iomanip>

#include <boost/graph/clustering_coefficient.hpp>
#include <boost/graph/bc_clustering.hpp>
#include <boost/graph/kruskal_min_spanning_tree.hpp>
#include <boost/graph/prim_minimum_spanning_tree.hpp>
#include <boost/graph/stoer_wagner_min_cut.hpp>
#include <boost/graph/iteration_macros.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>

#include "graph.hpp"

namespace NARO::Algo {

/**
 * @brief Calculate global clustering coefficient for a given graph
 *
 * @param[in] g A graph defined in `graph.hpp`
 * @returns a double value,
 */
float global_clustering_coefficient(NARO::Graph& g);

/**
 * @brief Calculate local clustering coefficient for a given graph
 *
 * @param[in] g A graph defined in `graph.hpp`
 * @returns A std::map contains local clustering coefficient for  each vertex. string label as the key.
 */
std::map<std::string, float> local_clustering_coefficient(NARO::Graph& g);

/**
 * @brief Find minimum spanning tree
 *
 * @param[in] g A graph defined in `graph.hpp`
 * @param[in] method the algorithm name for finding MST,  krustal or prim
 * @returns None
 */
void find_minimum_spanning_tree(NARO::Graph& g, const std::string& method);
/**
 * @brief A wrapper function for stoer wagner min_cut
 *
 * @param[in] g A graph defined in `graph.hpp`
 * @returns double, minmum cut weight
 */
double stoer_wagner_min_cut(NARO::Graph& g, bool verbose=false);

}

#endif /* graph_algo_hpp */
