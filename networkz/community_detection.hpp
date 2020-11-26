//
//  community_detection.hpp
//  networkz
//
//  Created by CAO Wei on 2020/11/18.
//  Copyright © 2020 曹巍. All rights reserved.
//

#ifndef community_detection_hpp
#define community_detection_hpp

#include <iostream>
#include <iomanip>

#include "graph.hpp"
#include "modularity.hpp"

namespace NARO::Algo::Community {

/**
 * @brief Calculate Newman's (generalized) modularity of a connected network
 *
 * @param[in] g A undirected graph.
 * @param[in] Q Newman's modularity.
 *
 * @TODO Not implemented yet
 *
 * Reference:
 *  M.E.J. Newman, "Modularity and community structure in networks",
 *  Proc. Natl. Acad. Sci. USA 103, 8577-8582(2006)
 */
//float newman_comm(NARO::Graph& g, float gamma);


/**
 * @brief Louvain community detections for a network
 *
 * @param[in] g A undirected graph.
 * @param[in] Q modularity.
 *
 * @TODO Not completed yet
 *
 * Reference:
 *  Blondel, Vincent D; Guillaume, Jean-Loup; Lambiotte, Renaud; Lefebvre, Etienne (9 October 2008).
 *  "Fast unfolding of communities in large networks".
 *  Journal of Statistical Mechanics: Theory and Experiment. 2008 (10): P10008.
 */
// Primary template
template<class QualityType>
struct Louvain
{
  typedef std::vector<int>::iterator v_int_iter;
  typedef std::vector<long double>::iterator v_long_double_iter;
  typedef std::pair<v_int_iter, v_long_double_iter> Neighbors;
  
  int number_pass; ///< The number of pass
  int neigh_last; ///< 
  int display_level = -2;
  int kmin = 1;
  
  long double precision = 0.000001L;
  long double alpha = 0.5L;
  long double sum_se = 0.0L;
  long double sum_sq = 0.0L;
  long double max_w = 1.0L;
  long double eps_impr;
  
  std::vector<long double> neigh_weight;
  std::vector<int> neigh_pos;
  
  QualityType* qual;
  
  Louvain(int number_pass, long double eps_impr, QualityType* q);
  
  void neigh_comm(int node);
  void partition2graph();
  void display_partition();
  QualityType::CSRgraph partition2graph_binary();
  bool one_level();
  void louvain();
};

}

#endif /* community_detection_hpp */
