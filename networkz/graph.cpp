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
                                      NARO::Edge& e,
                                      bool verbose=false)
{
  double w_degree = 0.0L;
  
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
