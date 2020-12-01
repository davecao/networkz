//
//  csr_graph.cpp
//  networkz
//
//  Created by 曹巍 on 2020/11/26.
//  Copyright © 2020 曹巍. All rights reserved.
//
#include <fstream>
#include "csr_graph.hpp"

namespace NARO::Algo::Community {
// -----------------------------------------------------------------------------
// Nested struct -- Modularity::CSRgraph
//  func: long double max_weight(int node);
//
long double CSRgraph::max_weight()
{
  long double max = 1.0L;

  if (weights.size()!=0)
    max = *(std::max_element)(weights.begin(), weights.end());
  return max;
}

// -----------------------------------------------------------------------------
// Nested struct -- Modularity::CSRgraph
//  func: void assign_weight(int node, int weight);
//
void CSRgraph::assign_weight(int node, int weight)
{
  sum_nodes_w -= nodes_w[node];
  nodes_w[node] = weight;
  sum_nodes_w += weight;
}

// -----------------------------------------------------------------------------
// Nested struct -- Modularity::CSRgraph
//  func: void add_selfloops();
//
void CSRgraph::add_selfloops()
{
  std::vector<unsigned long long> aux_deg;
  std::vector<int> aux_links;

  unsigned long long sum_d = 0ULL;

  for (int u=0 ; u < nb_nodes ; u++) {
    Neighbors p = neighbors(u);
    int deg = nb_neighbors(u);

    for (int i=0 ; i < deg ; i++) {
      int neigh = *(p.first+i);
      aux_links.push_back(neigh);
    }

    sum_d += (unsigned long long)deg;

    if (nb_selfloops(u) == 0.0L) {
      aux_links.push_back(u); // add a selfloop
      sum_d += 1ULL;
    }

    aux_deg.push_back(sum_d); // add the (new) degree of vertex u
  }

  links = aux_links;
  degrees = aux_deg;
  
  nb_links += (unsigned long long)nb_nodes;
}


// -----------------------------------------------------------------------------
// Nested struct -- Modularity::CSRgraph
//  func: void display(void);
//
void CSRgraph::display()
{
  for (int node=0 ; node<nb_nodes ; node++) {
    Neighbors p = neighbors(node);
    std::cout << node << ":" ;
    for (int i=0 ; i<nb_neighbors(node) ; i++) {
      if (true) {
        if (weights.size()!=0)
          std::cout << " (" << *(p.first+i) << " " << *(p.second+i) << ")";
        else
          std::cout << " " << *(p.first+i);
      }
    }
    std::cout << std::endl;
  }
}

// -----------------------------------------------------------------------------
// Nested struct -- Modularity::CSRgraph
//  func: void display_reverse(void);
//
void CSRgraph::display_reverse()
{
  for (int node=0 ; node<nb_nodes ; node++) {
    Neighbors p = neighbors(node);
    for (int i=0 ; i<nb_neighbors(node) ; i++) {
      if (node>*(p.first+i)) {
        if (weights.size()!=0)
          std::cout << *(p.first+i) << " " << node << " "
                    << *(p.second+i) << std::endl;
        else
          std::cout << *(p.first+i) << " " << node << std::endl;
      }
    }
  }
}

// -----------------------------------------------------------------------------
// Nested struct -- Modularity::CSRgraph
//  func: void display_binary(char *outfile);
//
void CSRgraph::display_binary(char *outfile)
{
  std::ofstream foutput;
  foutput.open(outfile , std::fstream::out | std::fstream::binary);

  foutput.write((char *)(&nb_nodes),sizeof(int));
  foutput.write((char *)(&degrees[0]),sizeof(unsigned long long)*nb_nodes);
  foutput.write((char *)(&links[0]),sizeof(int)*nb_links);
}

// -----------------------------------------------------------------------------
// Nested struct -- Modularity::CSRgraph
//  func: void check_symmetry();
//
bool CSRgraph::check_symmetry()
{
  int error = 0;
  for (int node=0 ; node<nb_nodes ; node++) {
    Neighbors p = neighbors(node);
    for (int i=0 ; i<nb_neighbors(node) ; i++) {
      int neigh = *(p.first+i);
      long double weight = *(p.second+i);

      Neighbors p_neigh = neighbors(neigh);
      for (int j=0 ; j<nb_neighbors(neigh) ; j++) {
        int neigh_neigh = *(p_neigh.first+j);
        long double neigh_weight = *(p_neigh.second+j);

        if (node==neigh_neigh && weight!=neigh_weight) {
          std::cout << node << " " << neigh << " "
                    << weight << " " << neigh_weight << std::endl;
          if (error++==10)
            exit(0);
        }
      }
    }
  }
  return (error==0);
}

// -----------------------------------------------------------------------------
// Nested struct -- Modularity::CSRgraph
//  func: int nb_neighbors(int node);
//
int CSRgraph::nb_neighbors(int node)
{
  assert(node>=0 && node<nb_nodes);

  if (node==0)
    return static_cast<int>(degrees[0]);
  else
    return (int)(degrees[node]-degrees[node-1]);
}

// -----------------------------------------------------------------------------
// Nested struct -- Modularity::CSRgraph
//  func: long double nb_selfloops(int node);
//
long double CSRgraph::nb_selfloops(int node)
{
  assert(node>=0 && node<nb_nodes);

  Neighbors p = neighbors(node);
  for (int i=0 ; i<nb_neighbors(node) ; i++) {
    if (*(p.first+i)==node) {
      if (weights.size()!=0)
        return (long double)*(p.second+i);
      else
        return 1.0L;
    }
  }
  return 0.0L;
}

// -----------------------------------------------------------------------------
// Nested struct -- Modularity::CSRgraph
//  func: long double weighted_degree(int node);
//
long double CSRgraph::weighted_degree(int node) {
  assert(node>=0 && node<nb_nodes);

  if (weights.size()==0)
    return (long double)nb_neighbors(node);
  else {
    Neighbors p = neighbors(node);
    long double res = 0.0L;
    for (int i=0 ; i<nb_neighbors(node) ; i++) {
      res += (long double)*(p.second+i);
    }
    return res;
  }
}

// -----------------------------------------------------------------------------
// Nested struct -- Modularity::CSRgraph
//  func: Neighbors neighbors(int node);
//
CSRgraph::Neighbors CSRgraph::neighbors(int node)
{
  assert(node>=0 && node<nb_nodes);

  if (node==0)
    return make_pair(links.begin(), weights.begin());
  else if (weights.size()!=0)
    return make_pair(links.begin()+degrees[node-1],
                     weights.begin()+degrees[node-1]);
  else
    return make_pair(links.begin()+degrees[node-1],
                     weights.begin());
}
}
