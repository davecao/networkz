//
//  csr_graph.hpp
//  networkz
//
//  Created by 曹巍 on 2020/11/26.
//  Copyright © 2020 曹巍. All rights reserved.
//

#ifndef csr_graph_hpp
#define csr_graph_hpp

#include <iostream>
#include <vector>
#include <assert.h>
#include <algorithm>  // std::max_element

namespace NARO::Algo::Community {

struct CSRgraph
{
  typedef std::vector<int>::iterator v_int_iter;
  typedef std::vector<long double>::iterator v_long_double_iter;
  typedef std::pair<v_int_iter, v_long_double_iter> Neighbors;

  int nb_nodes;
  unsigned long long nb_links;
  
  long double total_weight;
  int sum_nodes_w;
  
  std::vector<unsigned long long> degrees;
  std::vector<int> links;
  std::vector<long double> weights;
  
  std::vector<int> nodes_w;

  CSRgraph()
    : nb_nodes(0), nb_links(0ULL), total_weight(0.0L),sum_nodes_w(0)
  {}
  
  ~CSRgraph() {
    degrees.clear();
    links.clear();
    weights.clear();
    nodes_w.clear();
  }
  // return the biggest weight of links in the graph
  long double max_weight();

  // assign a weight to a node (needed after the first level)
  void assign_weight(int node, int weight);

  // add selfloop to each vertex in the graph
  void add_selfloops();

  void display(void);
  void display_reverse(void);
  void display_binary(char *outfile);
  bool check_symmetry();


  // return the number of neighbors (degree) of the node
  int nb_neighbors(int node);

  // return the number of self loops of the node
  long double nb_selfloops(int node);

  // return the weighted degree of the node
  long double weighted_degree(int node);

  // return pointers to the first neighbor and first weight of the node
  Neighbors neighbors(int node);
  // Graph CSR format
  //std::vector<int> xadj;
  //std::vector<int> adjncy;
  //std::vector<long double> xweights;
};

}
#endif /* csr_graph_hpp */
