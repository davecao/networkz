//
//  graph_algo.cpp
//  networkz
//
//  Created by 曹巍 on 2020/08/30.
//  Copyright © 2020 曹巍. All rights reserved.
//

#include "graph_algo.hpp"
// -----------------------------------------------------------------------------
void print(NARO::Graph& g) {
  NARO::VertexIter v, vend;
  for (boost::tie(v, vend) = vertices(g); v != vend; ++v) {
    std::cout<< g[*v].name << "("<< g[*v].weight <<")" << std::endl;
  }
  NARO::EdgeIter ei, ei_end;
  for (boost::tie(ei, ei_end) = boost::edges(g); ei != ei_end; ++ei){
    auto u = boost::source(*ei, g);
    auto v = boost::target(*ei, g);
    auto d = g[*ei].distance;
    std::cout<< g[u].name << "("<< g[u].weight <<")" << " -- " <<
                g[v].name << "("<< g[v].weight <<"): "<< d <<std::endl;
  }
}
// -----------------------------------------------------------------------------
//
float NARO::Algo::global_clustering_coefficient(Graph& g)
{
  // Compute the clustering coefficients of each vertex in the graph
  // and the mean clustering coefficient which is returned from the
  // computation.
  ClusteringContainer coefs(boost::num_vertices(g));
  ClusteringMap cm(coefs, g);
  float cc = boost::all_clustering_coefficients(g, cm);
  return cc;
}

// -----------------------------------------------------------------------------
//
std::map<std::string, float>
NARO::Algo::local_clustering_coefficient(Graph& g)
{
  std::map<std::string, float> cc;
  NARO::ClusteringContainer coefs(boost::num_vertices(g));
  NARO::ClusteringMap cm(coefs, g);
  float c = 0;
  NARO::VertexIter i, end;
  for (boost::tie(i, end) = boost::vertices(g); i != end; ++i) {
    // Vertex name
    std::string v_name = g[*i].name;
    c = boost::get(cm, *i);
    cc.insert(std::make_pair(v_name, c));
  }
  return cc;
}

// -----------------------------------------------------------------------------
// Boost minimum spanning tree algorithm
//   bundled property is not supported directly.
// TODO: Remove edges for Graph<listS, VecS> may get wrong result.
//
void NARO::Algo::find_minimum_spanning_tree(Graph& g,
                                            std::string& method)
{
  if (method == "kruskal") {
    std::vector<NARO::Edge> sp_e_tree(boost::num_edges(g));
    auto weightMap = boost::weight_map(boost::get(&NARO::gEdge::distance, g));
    boost::kruskal_minimum_spanning_tree(
                  g, &sp_e_tree[0], weightMap);
    //remove the edges not in kruskal's result
    NARO::EdgeIter ei, ei_end;
    for (boost::tie(ei, ei_end) = boost::edges(g); ei != ei_end; ++ei){
      //boost::remove_edge(boost::source(*ei,g), boost::target(*ei, g), g);
      auto it = std::find (sp_e_tree.begin(), sp_e_tree.end(), *ei);
      if (it == sp_e_tree.end()){
        boost::remove_edge(*ei, g);
      }
    }
  } else {
    std::vector<NARO::Vertex> sp_v_tree(boost::num_vertices(g));
    auto weightMap = boost::weight_map(boost::get(&NARO::gEdge::distance, g));
    boost::prim_minimum_spanning_tree(g, &sp_v_tree[0], weightMap);
    NARO::VertexIter v, vend;
    for (boost::tie(v, vend) = boost::vertices(g); v != vend; ++v) {
      auto it = std::find (sp_v_tree.begin(), sp_v_tree.end(), *v);
      if (it == sp_v_tree.end()){
        boost::clear_vertex(*v, g);
      }
    }
  }
}

// -----------------------------------------------------------------------------
// A wrapper of boost::stoer_wagner_min_cut

double NARO::Algo::stoer_wagner_min_cut(Graph& g)
{
  std::map<int, bool> parity;
  boost::associative_property_map<std::map<int, bool> > parities(parity);
  double w = boost::stoer_wagner_min_cut(g,
                                      boost::get(&NARO::gEdge::distance, g),
                                      boost::parity_map(parities));
  return w;
}
