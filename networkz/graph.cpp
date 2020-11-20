//
//  graph.cpp
//  networkz
//
//  Created by CAO Wei on 2020/07/30.
//  Copyright © 2020 曹巍. All rights reserved.
//

#include "graph.hpp"

// -----------------------------------------------------------------------------
// bilab::gVertex::to_graphviz()
//
std::string NARO::gVertex::to_graphviz() {
  std::stringstream ss;
    ss << "[label=\"" << name << "\", tpm=" << weight << "]";
    return ss.str();
}

// -----------------------------------------------------------------------------
// bilab::gEdge::to_graphviz()
//
std::string NARO::gEdge::to_graphviz() {
  std::stringstream ss;
  ss << "[weight=" << distance << ", "
  << "arrowhead=\"none\", color=\"purple\"]";
  return ss.str();
}

// -----------------------------------------------------------------------------
// bilab::gGraph::to_graphviz()
//
std::string NARO::gGraph::to_graphviz() {
  std::stringstream ss;
  ss << "label=\""<< glabel << "("<< get_local_time()<<")\";\n"
     << "node [shape=\"circle\", filled=\"none\", color=\"purple\"];\n"
     << "edge [arrowType=\"dot\", color=\"purple\"];\n";
  return ss.str();
}
// -----------------------------------------------------------------------------
// bilab::gGraph::get_hits_scores
//
// It contains hub score and authority score: Kleinberg (1999, 200)
//
//  1. The authority score of a node is a measure of the amount of valuable
//     information that this node holds.
//  2. The hub score of a node shows how many highly informative nodes or
//     authoritative nodes this node is pointing to.
//
// So a node with a high hub score shows that this node is pointing to many
// other authoritative nodes. On the other hand, a node with a high
// authoritative score shows that it is pointed to a large number of nodes,
// thus serves as a node of useful information in the network.
std::tuple<double, double>
get_hits(NARO::Graph& g, NARO::Vertex& v)
{
  double hub_score, authority_score;
  
  return std::forward_as_tuple(hub_score, authority_score);
}
// -----------------------------------------------------------------------------
// bilab::gGraph::get_total_weights
//
double NARO::get_total_weights(NARO::Graph& g, bool verbose=false){
  // Get the total weights of edges from the graph
  double tot = g[boost::graph_bundle].total_weights;
  if (tot == -1) {
    tot = 0.0L;
  }
  auto es = boost::edges(g);
  for (auto eit = es.first; eit != es.second; ++eit) {
    tot += g[*eit].distance;
    if (verbose) {
      std::cout << boost::source(*eit, g) << ' ' <<
                   boost::target(*eit, g) << '-' <<
                   g[*eit].distance << std::endl;
    }
  }
  return tot;
}
// -----------------------------------------------------------------------------
// bilab::gGraph::get_node_weighted_degree
//
double NARO::get_node_weighted_degree(NARO::Graph& g,
                                      NARO::Vertex& v,
                                      bool verbose=false)
{
  double w_degree = 0.0L;
  auto neighbours = boost::adjacent_vertices(v, g);
  for (auto neighbor : boost::make_iterator_range(neighbours)) {
    auto edge = boost::edge(v, neighbor, g).first;
    w_degree += g[edge].distance;
  }
  //NARO::VertexIter i, end;
  //for (boost::tie(i, end) = boost::vertices(g); i != end; ++i) {
    // Vertex name
  //  std::string v_name = g[*i].name;
    //c = boost::get(cm, *i);
    //cc.insert(std::make_pair(v_name, c));
  //}
  return w_degree;
}
///  self loops
double NARO::get_nb_selfloops(NARO::Graph& g,
                              NARO::Vertex& v,
                              bool verbose=false)
{
  double re = 0.0L;
  return re;
}
