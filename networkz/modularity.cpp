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


CSRgraph convert(NARO::Graph& g,
                 std::vector<int>* n2c,
                 std::vector<std::string>* lookup_table)
{
  CSRgraph csr_g;
  int node_size = static_cast<int>(boost::num_vertices(g));
  csr_g.nb_nodes = node_size;
  csr_g.sum_nodes_w = node_size;
  csr_g.degrees.resize(node_size);
  csr_g.nodes_w.assign(node_size, 1);
  std::vector<std::vector<std::pair<int, long double>>> links;
  // Add nodes' degrees
  NARO::VertexIter vi, vend;
  int node_id = 0;
  for(boost::tie(vi, vend) = boost::vertices(g); vi != vend; ++vi) {
    (*n2c)[node_id] = node_id;
    (*lookup_table)[node_id] = g[(*vi)].name;
    csr_g.degrees[node_id] = boost::degree(*vi, g);
    
    node_id++;
  }
  // cum_degree[0]=degree(0); cum_degree[1]=degree(0)+degree(1), etc.
  for (int i=1; i<csr_g.degrees.size(); i++) {
    csr_g.degrees[i] += csr_g.degrees[i-1];
  }
  // Read links: for each link (each link is counted twice)
  csr_g.nb_links = csr_g.degrees[csr_g.nb_nodes-1];
  csr_g.links.resize(csr_g.nb_links);
  csr_g.weights.resize(csr_g.nb_links);
  return csr_g;
}


/** ================================================================================
 *  struct Modularity
 *
 *  ================================================================================
 */
Modularity::Modularity(NARO::Graph& gr)
{
  g = convert(gr);
}
// -----------------------------------------------------------------------------
// struct Modularity::Modularity()
Modularity::Modularity(CSRgraph& gr)
{
  node_size = gr.nb_nodes;
  n2c.resize(node_size);
  in.resize(node_size);
  tot.resize(node_size);
  //lookup_table.resize(node_size);

  // initialization
  for (int i=0 ; i<size ; i++) {
    n2c[i] = i;
    in[i]  = g.nb_selfloops(i);
    tot[i] = g.weighted_degree(i);
  }
  
  // Map vertices to string map
  //for (auto vd : boost::make_iterator_range(boost::vertices(gr))) {
  //  name2vertex.insert(std::make_pair(gr[vd].name, vd));
  //  std::cout<< gr[vd].name << ":" << vd << std::endl;
  //}
  // Convert NARO::Graph to CSRgraph
  //convert(gr, &g, &n2c, &lookup_table);
  
  //this->g_ = &g;
  //bool verbose = false;
  
  //g[boost::graph_bundle].level = level;
  
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
//  NARO::VertexIter vi, vend;
//  int comm_id = 0;
//  for(boost::tie(vi, vend) = boost::vertices(gr); vi != vend; ++vi) {
//    // The component id
//    n2c[comm_id] = gr[*vi].communityId;
//    in[comm_id] = NARO::get_nb_selfloops(gr, *vi, verbose);
//    tot[comm_id] = NARO::get_node_weighted_degree(gr, *vi, verbose);
//    comm_id++;
//  }
  //long double w_d = NARO::get_community_degree(g, 0);
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
  //lookup_table.clear();
  // explicitly call deconstructor
  g.~CSRgraph();
  //rank.clear();
  //parent.clear();

  //delete components;
  //this->g_ = nullptr;
  //this->components = nullptr;
}

// -----------------------------------------------------------------------------
// struct Modularity::num_nodes()
int Modularity::num_nodes()
{
  return g.nb_nodes;
}

// -----------------------------------------------------------------------------
// struct Modularity::num_edges()
int Modularity::num_edges()
{
  return static_cast<int>(g.nb_links);
}

// -----------------------------------------------------------------------------
// Modularity::weighted_degree(int node)
//
long double Modularity::weighted_degree(int node)
{
  //long double w_degree = 0.0L;
  //auto neighbours = boost::adjacent_vertices(node, *this->g_);
  //for (auto neighbor : boost::make_iterator_range(neighbours)) {
  //  auto edge = boost::edge(node, neighbor, *this->g_).first;
  //  w_degree += (*this->g_)[edge].distance;
  //}
  //return w_degree;
  return g.weighted_degree(node);
}

// -----------------------------------------------------------------------------
// struct Modularity::total_weights()
//
long double Modularity::total_weights()
{
  //bool verbose = false;
  //double tot_w = NARO::get_total_weights(*g_, verbose);
  //if (tot_w == -1){
  //  tot_w = 0.0L;
  //  auto es = boost::edges(*g_);
  //  for (auto eit = es.first; eit != es.second; ++eit) {
  //    tot_w += (*g_)[*eit].distance;
  //  }
  //}
  //return tot_w;
  return g.total_weight;
}

// -----------------------------------------------------------------------------
// struct Modularity: remove()
void Modularity::remove(int node, int comm, long double dnodecomm)
{
  assert(node >= 0 && node < node_size);

  in[comm]  -= 2.0L * dnodecomm + g.nb_selfloops(node);
  tot[comm] -= g.weighted_degree(node);

  n2c[node] = -1;
}

// -----------------------------------------------------------------------------
// struct Modularity: insert()
void Modularity::insert(int node, int comm, long double dnodecomm)
{
  assert(node >= 0 && node < node_size);

  in[comm]  += 2.0L * dnodecomm + g.nb_selfloops(node);
  tot[comm] += g.weighted_degree(node);
    
  n2c[node] = comm;
}

// -----------------------------------------------------------------------------
// struct Modularity: gain()
long double Modularity::gain(int node, int comm, long double dnc,
                 long double degc)
{
  assert(node >= 0 && node < node_size);
    
  long double totc = tot[comm];
  long double m2 = g.total_weight;
    
  return (dnc - totc * degc / m2);
}

// -----------------------------------------------------------------------------
// struct Modularity: quality()
long double Modularity::quality()
{
  long double q  = 0.0L;
  long double m2 = g.total_weight;

  for (int i=0; i<this->node_size; i++) {
    if (tot[i] > 0.0L)
      q += in[i] - (tot[i]*tot[i]) / m2;
  }
  q /= m2;
  return q;
}

// -----------------------------------------------------------------------------
} // End of NARO::Algo::Community
