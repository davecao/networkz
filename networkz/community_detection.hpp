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

namespace NARO::Algo::modularity {

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
float newman_comm(NARO::Graph& g, float gamma);

struct Modularity
{
  int node_size;
  std::string name;
  std::vector<int> n2c;
  NARO::Graph* g_;
  // used to compute the quality participation of each community
  std::vector<long double> in, tot;
  
  Modularity(NARO::Graph& g, const std::string& n);
  ~Modularity();
  
  inline void remove(int node, int comm, long double dnodecomm);
  inline void insert(int node, int comm, long double dnodecomm);
  inline long double gain(int node, int comm, long double dnodecomm,
                          long double w_degree);
  long double quality();
};

template<class QualityType>
struct Louvain
{
  int number_pass;
  int neigh_last;
  long double eps_impr;
  std::vector<long double> neigh_weight;
  std::vector<int> neigh_pos;
  
  Louvain(NARO::Graph& g, int number_pass, long double eps_impr,
          QualityType* q);
  
  void neigh_comm(int node);
  void partition2graph();
  void display_partition();
  NARO::Graph partition2graph_binary();
  bool one_level();
};

}
#endif /* community_detection_hpp */
