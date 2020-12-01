//
//  modularity.hpp
//  networkz
//
//  Created by 曹巍 on 2020/11/23.
//  Copyright © 2020 曹巍. All rights reserved.
//

#ifndef modularity_hpp
#define modularity_hpp

#include <iostream>
#include <iomanip>

#include "graph.hpp"
#include "csr_graph.hpp"

namespace NARO::Algo::Community
{

struct Modularity
{
  std::string name = "Newman-Girvan Modularity";

  int node_size;
  CSRgraph* g;

  // Used to compute the quality participation of each community
  std::vector<int> n2c; ///< store nodes's community ids
  std::vector<long double> in;
  std::vector<long double> tot;

  Modularity(CSRgraph& gr);
  ~Modularity();

  void remove(int node, int comm, long double dnodecomm);
  void insert(int node, int comm, long double dnodecomm);
  long double gain(int node, int comm, long double dnodecomm,
                          long double w_degree);
  // Quality computation
  long double quality();

  // Graph operations with community ids
  long double weighted_degree(int node);
  long double total_weights();

};

} // End of NARO::Algo::Community
#endif /* modularity_hpp */
