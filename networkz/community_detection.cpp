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

  Neighbors p = (qual->g)->neighbors(node);
  int deg = (qual->g)->nb_neighbors(node);
  
  neigh_pos[0] = qual->n2c[node];
  neigh_weight[neigh_pos[0]] = 0;
  neigh_last = 1;
  
  for (int i=0 ; i<deg ; i++) {
    int neigh  = *(p.first+i);
    int neigh_comm = qual->n2c[neigh];
    long double neigh_w = ((qual->g)->weights.size()==0)?1.0L:*(p.second+i);
    
    if (neigh!=node) {
      if (neigh_weight[neigh_comm]==-1) {
        neigh_weight[neigh_comm] = 0.0L;
        neigh_pos[neigh_last++] = neigh_comm;
      }
      neigh_weight[neigh_comm] += neigh_w;
    }
  }
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
    Neighbors p = (qual->g)->neighbors(i);

    int deg = (qual->g)->nb_neighbors(i);
    for (int j=0 ; j<deg ; j++) {
      int neigh = *(p.first+j);
      std::cout << renumber[qual->n2c[i]] << " "
                << renumber[qual->n2c[neigh]] << std::endl;
    }
  }
}

// -----------------------------------------------------------------------------
// Louvain::display_partition()
template<class QualityType>
std::vector<int> Louvain<QualityType>::display_partition()
{
  std::vector<int> renumber(qual->node_size, -1);
  std::vector<int> node2community(qual->node_size, -1);
  for (int node = 0; node < qual->node_size; node++) {
    renumber[qual->n2c[node]]++;
  }

  int end=0;
  for (int i = 0; i < qual->node_size; i++){
    if (renumber[i] != -1){
      renumber[i] = end++;
    }
  }

  for (int i = 0; i < qual->node_size; i++) {
    node2community[i] = renumber[qual->n2c[i]];
    std::cout << i << " " << renumber[qual->n2c[i]] << std::endl;
  }
  return node2community;
}

// -----------------------------------------------------------------------------
// Louvain::partition2graph_binary()
//
template<class QualityType>
CSRgraph Louvain<QualityType>::partition2graph_binary()
{
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
    comm_weight[renumber[qual->n2c[node]]] += (qual->g)->nodes_w[node];
  }
  CSRgraph g;
  int nbc = static_cast<int>(comm_nodes.size());
  
  g.nb_nodes = static_cast<int>(comm_nodes.size());
  g.degrees.resize(nbc);
  g.nodes_w.resize(nbc);
  
  for (int comm=0 ; comm<nbc ; comm++) {
    std::map<int,long double> m;
    std::map<int,long double>::iterator it;

    int size_c = static_cast<int>(comm_nodes[comm].size());
    
    g.assign_weight(comm, comm_weight[comm]);
    
    for (int node=0; node<size_c; node++) {

      Neighbors p = (qual->g)->neighbors(comm_nodes[comm][node]);
      int deg = (qual->g)->nb_neighbors(comm_nodes[comm][node]);
      for (int i=0; i<deg; i++) {
        int neigh = *(p.first + i);
        int neigh_comm = renumber[qual->n2c[neigh]];
        long double neigh_weight = ((qual->g)->weights.size()==0) ? 1.0L : *(p.second+i);
        
        it = m.find(neigh_comm);
        if (it == m.end())
          m.insert(std::make_pair(neigh_comm, neigh_weight));
        else
          it->second += neigh_weight;
      }
    }
    g.degrees[comm] = (comm==0)?m.size() : g.degrees[comm-1] + m.size();
    g.nb_links += m.size();
    
    for (it = m.begin(); it != m.end(); it++) {
      g.total_weight += it->second;
      g.links.push_back(it->first);
      g.weights.push_back(it->second);
    }
  }
  return g;
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
  for (int i=0; i < qual->node_size; i++){
    random_order[i]=i;
  }
  for (int i=0 ; i < qual->node_size-1; i++) {
    int rand_pos = rand()%(qual->node_size-i)+i;
    int tmp = random_order[i];
    random_order[i] = random_order[rand_pos];
    random_order[rand_pos] = tmp;
  }

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
  } while (nb_moves > 0 && new_quality - curr_quality > eps_impr);

  return improvement;
}

// -----------------------------------------------------------------------------
// Louvain::louvain()
//    Run the louvain algorithm.
//
template<class QualityType>
std::tuple<double, int> Louvain<QualityType>::louvain(std::vector<int>& n2c)
{
  bool improvement = true;
  bool verbose = true;
  long double quality = qual->quality();
  long double new_qual;
  //int display_level = -2;
  unsigned short nb_calls = 1;
  int level = 0;
  std::vector<std::vector<int>> levels;

  CSRgraph g;
  auto c = *this;

  do {
    if (verbose) {
      std::cout << "level " << level << ":\n";
      std::cout << "  network size: "
        << (c.qual)->g->nb_nodes << " nodes, "
        << (c.qual)->g->nb_links << " links, "
        << (c.qual)->g->total_weight << " weight" << std::endl;
    }
    improvement = c.one_level();
    new_qual = (c.qual)->quality();

    auto node2comm = c.display_partition();
    levels.push_back(node2comm);
    level++;
    // update the community ids for all nodes
    // @TODO Slowing process because of returning a copy of the object
    g = c.partition2graph_binary();
    if (nb_calls > 0) {
      //delete qual;
      qual = nullptr;
      qual = new NARO::Algo::Community::Modularity(g);
    }
    nb_calls++;

    // recursive
    c = Louvain<QualityType>(-1, precision, qual);
    
    if (verbose) {
      std::cout <<"  quality increased from " << quality << " to "
                << new_qual << std::endl;
    }
    quality = new_qual;

  } while (improvement);
  
  // Assign community to all nodes
  int display_level = static_cast<int>(levels.size()) - 1;
  n2c.resize(levels[0].size());

  for (unsigned int i=0 ; i<levels[0].size() ; i++){
    n2c[i] = i;
  }
      
  for (int l=0 ; l < display_level ; l++) {
    for (unsigned int node=0 ; node<levels[0].size() ; node++) {
      n2c[node] = levels[l][n2c[node]];
    }
  }
  return std::forward_as_tuple(quality, static_cast<int>(levels.size()));
}
// Specialization
template struct Louvain<Modularity>;
} // end of NARO::Algo::modularity
