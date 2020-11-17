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
float NARO::Algo::global_clustering_coefficient(NARO::Graph& g)
{
  // Compute the clustering coefficients of each vertex in the graph
  // and the mean clustering coefficient which is returned from the
  // computation.
  NARO::ClusteringContainer coefs(boost::num_vertices(g));
  NARO::ClusteringMap cm(coefs, g);
  float cc = boost::all_clustering_coefficients(g, cm);
  return cc;
}

// -----------------------------------------------------------------------------
//
std::map<std::string, float>
NARO::Algo::local_clustering_coefficient(NARO::Graph& g)
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
void NARO::Algo::find_minimum_spanning_tree(NARO::Graph& g,
                                            const std::string& method)
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
// Prize-Collecting Steiner Forest (PCSF)
// Not Finished yet.
void NARO::Algo::prize_collecting_steiner_forest(NARO::Graph& g,
                                                 int num_clusters,
                                                 NARO::Graph& sub)
{
  // Get the minimum spanning tree by prim algorithm
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
  
  weightMap = boost::weight_map(boost::get(&NARO::gEdge::distance, g));
  // 1. Compute the distance matrix by the dijkstra algorithm
  
  //boost::dijkstra_shortest_paths
}
// -----------------------------------------------------------------------------
// A wrapper of boost::stoer_wagner_min_cut
//void test_stoer_wagner_min_cut(NARO::Graph& gr, double* w);

double NARO::Algo::stoer_wagner_min_cut(NARO::Graph& g, bool verbose)
{
  //test_stoer_wagner_min_cut(g, &w);
  std::map<int, bool> parity;
  boost::associative_property_map<std::map<int, bool> > parities(parity);
  double w = boost::stoer_wagner_min_cut(g,
                                      boost::get(&NARO::gEdge::distance, g),
                                      boost::parity_map(parities));
  std::cout << "The min-cut weight of G is " << w << ".\n" << std::endl;
  std::cout << "One set of vertices consists of:" << std::endl;
  size_t i;
  for (i = 0; i < boost::num_vertices(g); ++i)
  {
      if (boost::get(parities, i))
          std::cout << i << std::endl;
  }
  std::cout << std::endl;

  std::cout << "The other set of vertices consists of:" << std::endl;
  for (i = 0; i < boost::num_vertices(g); ++i)
  {
      if (!boost::get(parities, i))
          std::cout << i << std::endl;
  }
  std::cout << std::endl;
  return w;
}

// ----
void test_stoer_wagner_min_cut(NARO::Graph& gr, double* w)
{
  struct edge_t
  {
      unsigned long first;
      unsigned long second;
  };
  typedef boost::adjacency_list< boost::vecS, boost::vecS, boost::undirectedS,
      boost::no_property, boost::property< boost::edge_weight_t, int > >
      undirected_graph;
  typedef boost::property_map< undirected_graph, boost::edge_weight_t >::type
      weight_map_type;
  typedef boost::property_traits< weight_map_type >::value_type weight_type;

  // define the 16 edges of the graph. {3, 4} means an undirected edge between
  // vertices 3 and 4.
  edge_t edges[] = { { 3, 4 }, { 3, 6 }, { 3, 5 }, { 0, 4 }, { 0, 1 },
      { 0, 6 }, { 0, 7 }, { 0, 5 }, { 0, 2 }, { 4, 1 }, { 1, 6 }, { 1, 5 },
      { 6, 7 }, { 7, 5 }, { 5, 2 }, { 3, 4 } };

  // for each of the 16 edges, define the associated edge weight. ws[i] is the
  // weight for the edge that is described by edges[i].
  weight_type ws[] = { 0, 3, 1, 3, 1, 2, 6, 1, 8, 1, 1, 80, 2, 1, 1, 4 };

  // construct the graph object. 8 is the number of vertices, which are
  // numbered from 0 through 7, and 16 is the number of edges.
  undirected_graph g(edges, edges + 16, ws, 8, 16);
  BOOST_AUTO(parities,
  boost::make_one_bit_color_map(
      boost::num_vertices(g), boost::get(boost::vertex_index, g)));
  *w = boost::stoer_wagner_min_cut(
                                      g,
                                      boost::get(boost::edge_weight, g),
                                      boost::parity_map(parities));
  std::ofstream graphfile("stoer_wagner.dot");
  boost::write_graphviz(graphfile, g);
  std::cout << "The min-cut weight of G is " << *w << ".\n" << std::endl;
  std::cout << "One set of vertices consists of:" << std::endl;
  size_t i;
  for (i = 0; i < boost::num_vertices(g); ++i)
  {
      if (boost::get(parities, i))
          std::cout << i << std::endl;
  }
  std::cout << std::endl;

  std::cout << "The other set of vertices consists of:" << std::endl;
  for (i = 0; i < boost::num_vertices(g); ++i)
  {
      if (!boost::get(parities, i))
          std::cout << i << std::endl;
  }
  typedef boost::exterior_vertex_property<undirected_graph, float> ClusteringProperty;
  /// clustering container
  typedef ClusteringProperty::container_type ClusteringContainer;
  /// clustering map
  typedef ClusteringProperty::map_type ClusteringMap;
  ClusteringContainer coefs(boost::num_vertices(g));
  ClusteringMap cm(coefs, g);
  double cc = boost::mean_clustering_coefficient(g, cm);
  std::cout << "Mean clustering coefficient: " << cc << std::endl;
  std::cout << std::endl;
}

