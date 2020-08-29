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
