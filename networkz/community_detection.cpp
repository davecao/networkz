//
//  community_detection.cpp
//  networkz
//
//  Created by CAO Wei on 2020/11/18.
//  Copyright © 2020 曹巍. All rights reserved.
//

#include "community_detection.hpp"

namespace  NARO::Algo::modularity
{

// -----------------------------------------------------------------------------
// Newman's modularity
float newman_comm(NARO::Graph& g, float gamma)
{
  return 0.0;
}

// -----------------------------------------------------------------------------
// struct Modularity: Modularity()
Modularity::Modularity(NARO::Graph& g, const std::string& n)
{
  int size = static_cast<int>(boost::num_vertices(g));
  n2c.resize(size);
  in.resize(size);
  tot.resize(size);
  this->g_ = &g;
  
}
// -----------------------------------------------------------------------------
// struct Modularity: ~Modularity()
Modularity::~Modularity()
{
  n2c.clear();
  in.clear();
  tot.clear();
  
}
// -----------------------------------------------------------------------------
// struct Modularity: remove()
inline void Modularity::remove(int node, int comm, long double dnodecomm)
{
  assert(node >= 0 && node < node_size);

  in[comm]  -= 2.0L * dnodecomm + this->g_.nb_selfloops(node);
  tot[comm] -= this->g_.weighted_degree(node);
    
  n2c[node] = -1;
}
// -----------------------------------------------------------------------------
// struct Modularity: insert()
inline void Modularity::insert(int node, int comm, long double dnodecomm)
{
  assert(node >= 0 && node < node_size);
    
  in[comm]  += 2.0L*dnodecomm + this->g_.nb_selfloops(node);
  tot[comm] += this->g_.weighted_degree(node);
    
  n2c[node] = comm;
}

// -----------------------------------------------------------------------------
// struct Modularity: gain()
inline long double Modularity::gain(int node, int comm, long double dnc,
                 long double degc)
{
  assert(node >= 0 && node < node_size);
    
  long double totc = tot[comm];
  long double m2   = this->g_.total_weight;
    
  return (dnc - totc * degc / m2);
}
// -----------------------------------------------------------------------------
// struct Modularity: quality()
long double Modularity::quality()
{
  long double q  = 0.0L;
  long double m2 = g.total_weight;

  for (int i=0 ; i<node_size ; i++) {
    if (tot[i] > 0.0L)
      q += in[i] - (tot[i]*tot[i]) / m2;
  }
  q /= m2;
  return q;
}
// -----------------------------------------------------------------------------
// Louvain's modularity



} // end of NARO::Algo::modularity
