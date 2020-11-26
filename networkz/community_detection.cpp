//
//  community_detection.cpp
//  networkz
//
//  Created by CAO Wei on 2020/11/18.
//  Copyright © 2020 曹巍. All rights reserved.
//
#include <numeric>
#include <random>

#include "community_detection.hpp"

namespace NARO::Algo::Community
{
// -----------------------------------------------------------------------------
// Newman's modularity
//float newman_comm(NARO::Graph& g, float gamma)
//{
//  return 0.0;
//}



// -----------------------------------------------------------------------------
// Louvain's Constructor
//
template<class QualityType>
Louvain<QualityType>::Louvain(
                              int number_pass,
                              long double eps_impr,
                              QualityType* q)
{
  this->qual = q;
  
  this->neigh_weight.resize(qual->node_size, -1);
  this->neigh_pos.resize(qual->node_size);
  this->neigh_last = 0;
  
  this->number_pass = number_pass;
  this->eps_impr = eps_impr;
}

// -----------------------------------------------------------------------------
// Louvain::neigh_comm()
template<class QualityType>
void Louvain<QualityType>::neigh_comm(int node)
{
  for (int i=0 ; i<neigh_last ; i++)
    neigh_weight[neigh_pos[i]]=-1;
  neigh_last = 0;

  //auto neighbours = boost::adjacent_vertices(node, *qual->g_);
  //auto degree = boost::degree(node, *qual->g_);
  Neighbors p = (qual->g).neighbors(node);
  int deg = (qual->g).nb_neighbors(node);
  
  neigh_pos[0] = qual->n2c[node];
  neigh_weight[neigh_pos[0]] = 0;
  neigh_last = 1;
  
  for (int i=0 ; i<deg ; i++) {
    int neigh  = *(p.first+i);
    int neigh_comm = qual->n2c[neigh];
    long double neigh_w = ((qual->g).weights.size()==0)?1.0L:*(p.second+i);
    
    if (neigh!=node) {
      if (neigh_weight[neigh_comm]==-1) {
        neigh_weight[neigh_comm] = 0.0L;
        neigh_pos[neigh_last++] = neigh_comm;
      }
      neigh_weight[neigh_comm] += neigh_w;
    }
  }
  /*
  for (auto neighbor : boost::make_iterator_range(neighbours)) {
    int neigh_comm = qual->n2c[neighbor];
    auto edge = boost::edge(node, neighbor, *qual->g_).first;
    long double neight_w = (*qual->g_)[edge].distance;
    if (neighbor != node) {
      if (neigh_weight[neigh_comm] == -1) {
        neigh_weight[neigh_comm] = 0.0L;
        neigh_pos[neigh_last++] = neigh_comm;
      }
      neigh_weight[neigh_comm] += neight_w;
    }
  }
   */
}

// -----------------------------------------------------------------------------
// Louvain::partition2graph()
template<class QualityType>
void Louvain<QualityType>::partition2graph()
{
  std::vector<int> renumber(qual->node_size, -1);
  for (int node = 0; node < qual->node_size; node++) {
    renumber[qual->n2c[node]]++;
  }
  int end = 0;
  for (int i=0 ; i< qual->node_size ; i++){
    if (renumber[i] != -1){
      renumber[i]=end++;
    }
  }
  for (int i=0 ; i< qual->node_size ; i++) {
    Neighbors p = (qual->g).neighbors(i);

    int deg = (qual->g).nb_neighbors(i);
    for (int j=0 ; j<deg ; j++) {
      int neigh = *(p.first+j);
      std::cout << renumber[qual->n2c[i]] << " "
                << renumber[qual->n2c[neigh]] << std::endl;
    }
  }
  /*
  for (int i=0 ; i< qual->node_size ; i++) {
    auto neighbours = boost::adjacent_vertices(i, *qual->g_);
    for (auto neighbor : boost::make_iterator_range(neighbours)) {
      std::cout << renumber[qual->n2c[i]] << " "
                << renumber[qual->n2c[neighbor]] << std::endl;
    }
  }
   */
}

// -----------------------------------------------------------------------------
// Louvain::display_partition()
template<class QualityType>
void Louvain<QualityType>::display_partition()
{
  std::vector<int> renumber(qual->node_size, -1);
  for (int node=0 ; node < qual->node_size ; node++) {
    renumber[qual->n2c[node]]++;
  }

  int end=0;
  for (int i=0 ; i < qual->node_size ; i++)
    if (renumber[i]!=-1)
      renumber[i] = end++;

  for (int i=0 ; i < qual->node_size ; i++)
    std::cout << i << " " << renumber[qual->n2c[i]] << std::endl;
}

// -----------------------------------------------------------------------------
// Louvain::partition2graph_binary()
//
template<class QualityType>
CSRgraph Louvain<QualityType>::partition2graph_binary()
{
  CSRgraph* sub = nullptr;
  // Renumber communities
  std::vector<int> renumber(qual->node_size, -1);
  for (int node=0 ; node < qual->node_size ; node++)
    renumber[qual->n2c[node]]++;
  
  int last = 0;
  for (int i=0; i < qual->node_size; i++) {
    if (renumber[i] != -1)
      renumber[i] = last++;
  }
  // Compute communities
  std::vector<std::vector<int> > comm_nodes(last);
  std::vector<int> comm_weight(last, 0);
  
  for (int node = 0 ; node < (qual->node_size) ; node++) {
    comm_nodes[renumber[qual->n2c[node]]].push_back(node);
    
    //comm_weight[renumber[qual->n2c[node]]] += (qual->g_).nodes_w[node];
  }

  return *sub;
}

// -----------------------------------------------------------------------------
// Louvain::one_level()
//
template<class QualityType>
bool Louvain<QualityType>::one_level()
{
  bool improvement = false;
  int nb_moves;
  int nb_pass_done = 0;
  long double new_quality = qual->quality();
  long double curr_quality = new_quality;
  
  std::vector<int> random_order(qual->node_size);
  //std::for_each(random_order.begin(),
  //              random_order.end(),
  //              [i=0] (int& x) mutable {x = i++;});
  //
  //std::generate(random_order.begin(),
  //              random_order.end(),
  //              [n=0] () mutable { return n++;});
  // Initial a node sequence, 0 ... n
  std::iota(random_order.begin(), random_order.end(), 0);

  // Shuffle the nodes with a random engine
  std::random_device seed_gen;
  std::mt19937_64 engine(seed_gen());
  std::shuffle(random_order.begin(), random_order.end(), engine);
  
  //int n_vertices = static_cast<int>(boost::num_vertices(*qual->g_));
  //std::vector<int> component(n_vertices);
  //int num = boost::connected_components(*qual->g_, &component[0]);

  do {
    curr_quality = new_quality;
    nb_moves = 0;
    nb_pass_done++;
    
    // each node in a connected component network:
    //   1. remove the node from its community.
    //   2. insert it in the best community.
    //
    for (int node_tmp = 0 ; node_tmp < qual->node_size ; node_tmp++) {
      int node = random_order[node_tmp];
      int node_comm = qual->n2c[node];
      long double w_degree = qual->weighted_degree(node);
      // computation of all neighboring communities of current node
      neigh_comm(node);
      // remove the node from its current community
      qual->remove(node, node_comm, neigh_weight[node_comm]);
      // Compute the nearest community for the node
      int best_comm = node_comm;
      long double best_nblinks = 0.0L;
      long double best_increase = 0.0L;
      for (int i=0; i<neigh_last; i++) {
        long double increase = qual->gain(node,
                                          neigh_pos[i],
                                          neigh_weight[neigh_pos[i]],
                                          w_degree);
        if (increase > best_increase) {
          best_comm = neigh_pos[i];
          best_nblinks = neigh_weight[neigh_pos[i]];
          best_increase = increase;
        }
      }
      // insert node in the nearest community
      qual->insert(node, best_comm, best_nblinks);
      if (best_comm != node_comm) {
        nb_moves++;
      }
    }
    new_quality = qual->quality();
    if (nb_moves > 0)
      improvement = true;
    //NARO::VertexIter v, vend;
    //for (boost::tie(v, vend) = boost::vertices(*qual->g_); v != vend; ++v) {
      //std::cout<< g[*v].name << "("<< g[*v].weight <<")" << std::endl;
    //}
  } while (nb_moves > 0 && new_quality - curr_quality > eps_impr);

  return improvement;
}

// -----------------------------------------------------------------------------
// Louvain::louvain()
//    Run the louvain algorithm.
//
template<class QualityType>
void Louvain<QualityType>::louvain()
{
  bool improvement = true;
  bool verbose = false;
  long double quality = qual->quality();
  long double new_qual;

  unsigned short nb_calls = 0;
  int level = 0;
  
  do {
    if (verbose) {
      std::cout << "level " << level << ":\n";
      //display_time("  start computation");
      std::cout << "  network size: "
        << qual->num_nodes() << " nodes, "
        << qual->num_edges() << " links, "
        << qual->total_weights() << " weight" << std::endl;
    }
    improvement = this->one_level();
    new_qual = this->qual->quality();
    // increase the level
    level++;
    // update the community ids for all nodes
    g = this->partition2graph_binary();
    
    nb_calls++;
    // recursive
    
    if (verbose) {
      std::cout <<"  quality increased from " << quality << " to "
                << new_qual << std::endl;
    }
    quality = new_qual;

  } while (improvement);
}
// Specialization
template struct Louvain<Modularity>;
} // end of NARO::Algo::modularity
