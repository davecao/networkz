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

namespace NARO::Algo::Community
{

struct Modularity
{
  std::string name = "Newman-Girvan Modularity";

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

  int node_size;
  int edge_size;

  //NARO::Graph* g_;
  CSRgraph g;

  // Store graph connected components
  //std::vector<NARO::VertexIndex> rank;
  //std::vector<NARO::Vertex> parent;
  //boost::disjoint_sets<NARO::Rank, NARO::Parent> ds;
  //NARO::Components* components;
  // Used to compute the quality participation of each community
  std::vector<int> n2c; ///< store nodes's community ids
  std::vector<long double> in;
  std::vector<long double> tot;
  std::vector<std::string> lookup_table;
    
  
  Modularity(NARO::Graph& gr);
  ~Modularity();

  void remove(int node, int comm, long double dnodecomm);
  void insert(int node, int comm, long double dnodecomm);
  long double gain(int node, int comm, long double dnodecomm,
                          long double w_degree);
  // Quality computation
  long double quality();

  // Graph operations with community ids
  int num_nodes();
  int num_edges();
  long double weighted_degree(int node);
  long double total_weights();

};

} // End of NARO::Algo::Community
#endif /* modularity_hpp */
