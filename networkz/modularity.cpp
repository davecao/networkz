//
//  modularity.cpp
//  networkz
//
//  Created by 曹巍 on 2020/11/23.
//  Copyright © 2020 曹巍. All rights reserved.
//
#include <boost/foreach.hpp>
#include "modularity.hpp"

namespace NARO::Algo::Community
{
// -----------------------------------------------------------------------------
// test
void test_conn_compnent(NARO::Graph& g)
{
  int n_vertices = static_cast<int>(boost::num_vertices(g));
  std::vector<NARO::Vertex> parent(n_vertices);

  NARO::Components components(parent.begin(), parent.end());
  // Iterate through the component indices
  BOOST_FOREACH (VertexIndex current_index, components)
  {
    std::cout << "component " << current_index << " contains: ";
    // Iterate through the child vertex indices for [current_index]
    BOOST_FOREACH (VertexIndex child_index, components[current_index])
    {
      std::cout << child_index << " ";
    }
  std::cout << std::endl;
  }
}

/** ================================================================================
 *  struct Modularity
 *
 *  ================================================================================
 */

// -----------------------------------------------------------------------------
// struct Modularity::Modularity()
Modularity::Modularity(CSRgraph& gr)
{
  g = &gr;
  node_size = g->nb_nodes;
  n2c.resize(node_size);
  in.resize(node_size);
  tot.resize(node_size);
  //lookup_table.resize(node_size);

  // initialization
  for (int i=0 ; i<node_size ; i++) {
    n2c[i] = i;
    in[i]  = g->nb_selfloops(i);
    tot[i] = g->weighted_degree(i);
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
  g = nullptr;
}

// -----------------------------------------------------------------------------
// Modularity::weighted_degree(int node)
//
long double Modularity::weighted_degree(int node)
{
  return g->weighted_degree(node);
}

// -----------------------------------------------------------------------------
// struct Modularity::total_weights()
//
long double Modularity::total_weights()
{
  return g->total_weight;
}

// -----------------------------------------------------------------------------
// struct Modularity: remove()
void Modularity::remove(int node, int comm, long double dnodecomm)
{
  assert(node >= 0 && node < node_size);

  in[comm]  -= 2.0L * dnodecomm + g->nb_selfloops(node);
  tot[comm] -= g->weighted_degree(node);

  n2c[node] = -1;
}

// -----------------------------------------------------------------------------
// struct Modularity: insert()
void Modularity::insert(int node, int comm, long double dnodecomm)
{
  assert(node >= 0 && node < node_size);

  in[comm]  += 2.0L * dnodecomm + g->nb_selfloops(node);
  tot[comm] += g->weighted_degree(node);
    
  n2c[node] = comm;
}

// -----------------------------------------------------------------------------
// struct Modularity: gain()
long double Modularity::gain(int node, int comm, long double dnc,
                 long double degc)
{
  assert(node >= 0 && node < node_size);
    
  long double totc = tot[comm];
  long double m2 = g->total_weight;
    
  return (dnc - totc * degc / m2);
}

// -----------------------------------------------------------------------------
// struct Modularity: quality()
long double Modularity::quality()
{
  long double q  = 0.0L;
  long double m2 = g->total_weight;

  for (int i=0; i<this->node_size; i++) {
    if (tot[i] > 0.0L)
      q += in[i] - (tot[i]*tot[i]) / m2;
  }
  q /= m2;
  return q;
}

// -----------------------------------------------------------------------------
} // End of NARO::Algo::Community
