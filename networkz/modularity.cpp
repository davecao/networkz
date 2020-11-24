//
//  modularity.cpp
//  networkz
//
//  Created by 曹巍 on 2020/11/23.
//  Copyright © 2020 曹巍. All rights reserved.
//

#include "modularity.hpp"

namespace NARO::Algo::Community
{
// -----------------------------------------------------------------------------
// struct Modularity::Modularity()
Modularity::Modularity(NARO::Graph& g, int level)
{
  this->node_size = static_cast<int>(boost::num_vertices(g));
  n2c.resize(node_size);
  in.resize(node_size);
  tot.resize(node_size);

  this->g_ = &g;
  bool verbose = false;

  g[boost::graph_bundle].level = level;

  // Component id
  int n_vertices = static_cast<int>(boost::num_vertices(g));
  std::vector<int> component(n_vertices);
  int num = boost::connected_components(g, &component[0]);
  
  // initialization
  NARO::VertexIter vi, vend;
  int comm_id = 0;
  
  for(boost::tie(vi, vend) = boost::vertices(g); vi != vend; ++vi) {
    n2c[comm_id] = comm_id;

    // Initialization of community ids/component ids for nodes
    g[*vi].community_id = comm_id;
    g[*vi].component_id = component[*vi];
    
    in[comm_id] = NARO::get_nb_selfloops(g, *vi, verbose);
    tot[comm_id] = NARO::get_node_weighted_degree(g, *vi, verbose);
    comm_id++;
  }
}

// -----------------------------------------------------------------------------
// struct Modularity::~Modularity()
Modularity::~Modularity()
{
  // explicitly clear the memory
  n2c.clear();
  in.clear();
  tot.clear();
  this->g_ = nullptr;
}

// -----------------------------------------------------------------------------
// struct Modularity::num_nodes()
int Modularity::num_nodes()
{
  return static_cast<int>(boost::num_vertices(*this->g_));
}

// -----------------------------------------------------------------------------
// struct Modularity::num_edges()
int Modularity::num_edges()
{
  return static_cast<int>(boost::num_edges(*this->g_));
}

// -----------------------------------------------------------------------------
// Modularity::weighted_degree(int node)
//
long double Modularity::weighted_degree(int node)
{
  long double w_degree = 0.0L;
  auto neighbours = boost::adjacent_vertices(node, *this->g_);
  for (auto neighbor : boost::make_iterator_range(neighbours)) {
    auto edge = boost::edge(node, neighbor, *this->g_).first;
    w_degree += (*this->g_)[edge].distance;
  }
  return w_degree;
}

// -----------------------------------------------------------------------------
// struct Modularity::total_weights()
//
long double Modularity::total_weights()
{
  bool verbose = false;
  double tot_w = NARO::get_total_weights(*g_, verbose);
  if (tot_w == -1){
    tot_w = 0.0L;
    auto es = boost::edges(*g_);
    for (auto eit = es.first; eit != es.second; ++eit) {
      tot_w += (*g_)[*eit].distance;
    }
  }
  return tot_w;
}

// -----------------------------------------------------------------------------
// struct Modularity: remove()
void Modularity::remove(int node, int comm, long double dnodecomm)
{
  assert(node >= 0 && node < node_size);
  bool verbose = false;
  unsigned long v = static_cast<unsigned long>(node);
  in[comm]  -= 2.0L * dnodecomm +
    NARO::get_nb_selfloops(*(this->g_),v, verbose);
  tot[comm] -= NARO::get_node_weighted_degree(*(this->g_), v, verbose);

  n2c[node] = -1;
}

// -----------------------------------------------------------------------------
// struct Modularity: insert()
void Modularity::insert(int node, int comm, long double dnodecomm)
{
  assert(node >= 0 && node < node_size);
  bool verbose = false;
  unsigned long v = static_cast<unsigned long>(node);

  in[comm]  += 2.0L * dnodecomm +
    NARO::get_nb_selfloops(*(this->g_),v, verbose);
  tot[comm] += NARO::get_node_weighted_degree(*(this->g_), v, verbose);
    
  n2c[node] = comm;
}

// -----------------------------------------------------------------------------
// struct Modularity: gain()
long double Modularity::gain(int node, int comm, long double dnc,
                 long double degc)
{
  assert(node >= 0 && node < node_size);
    
  long double totc = tot[comm];
  long double m2   =
    (*(this->g_))[boost::graph_bundle].total_weights;
    
  return (dnc - totc * degc / m2);
}

// -----------------------------------------------------------------------------
// struct Modularity: quality()
long double Modularity::quality()
{
  long double q  = 0.0L;
  long double m2 = (*(this->g_))[boost::graph_bundle].total_weights * 2;

  for (int i=0; i<this->node_size; i++) {
    if (tot[i] > 0.0L)
      q += in[i] - (tot[i]*tot[i]) / m2;
  }
  q /= m2;
  return q;
}

} // End of NARO::Algo::Community
