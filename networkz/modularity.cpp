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
  
  // Get connected components in the graph
  //rank.resize(node_size);
  //parent.resize(node_size);
  
  //boost::disjoint_sets<NARO::Rank, NARO::Parent> ds(&rank[0], &parent[0]);
  //boost::initialize_incremental_components(g, ds);
  //boost::incremental_components(g, ds);
  //components = new NARO::Components(parent.begin(), parent.end());
  
  // Component id
  //std::vector<int> component(node_size);
  //int num = boost::connected_components(g, &component[0]);
  
  // initialization
  //NARO::VertexIter vi, vend;
  //int comm_id = 0;
  
  /*
  // Iterate through the component indices
  for (int comp_id=0; comp_id < num; comp_id++) {
    // loop over vertices
    comm_id = 0; // reset community id to zero
    for(boost::tie(vi, vend) = boost::vertices(g); vi != vend; ++vi) {
      if (component[*vi] == comp_id) {
        // The component id
        g[*vi].component_id = component[*vi];
        n2c[comm_id] = comm_id;
        // Initialization of community ids/component ids for nodes
        g[*vi].community_id = comm_id;
        comm_id++;
      }
      in[comm_id] = NARO::get_nb_selfloops(g, *vi, verbose);
      tot[comm_id] = NARO::get_node_weighted_degree(g, *vi, verbose);
    }
  }
  */
  // initialization
  NARO::VertexIter vi, vend;
  int comm_id = 0;
  for(boost::tie(vi, vend) = boost::vertices(g); vi != vend; ++vi) {
    // The component id
    n2c[comm_id] = g[*vi].communityId;
    in[comm_id] = NARO::get_nb_selfloops(g, *vi, verbose);
    tot[comm_id] = NARO::get_node_weighted_degree(g, *vi, verbose);
    comm_id++;
  }
  // Test components
  //test_conn_compnent(g);
  //
  //NARO::by_community_idx_t cInx;
  //cInx = NARO::get_community_byId(g, 0, false);
  //for (auto& vertex : cInx) {
  //  std::cout << vertex.name << " "
  //            << vertex.componentId << " "
  //            << vertex.communityId << "\n";
  //}
}

// -----------------------------------------------------------------------------
// struct Modularity::~Modularity()
Modularity::~Modularity()
{
  // explicitly clear the memory
  n2c.clear();
  in.clear();
  tot.clear();
  //rank.clear();
  //parent.clear();

  //delete components;

  this->g_ = nullptr;
  //this->components = nullptr;
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
