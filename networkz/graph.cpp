//
//  graph.cpp
//  networkz
//
//  Created by CAO Wei on 2020/07/30.
//  Copyright © 2020 曹巍. All rights reserved.
//

#include "graph.hpp"

// -----------------------------------------------------------------------------
// bilab::gVertex::operator==()
//
bool NARO::operator==(const NARO::gVertex& lhs, const NARO::gVertex& rhs)
{
  return lhs.name == rhs.name;
}

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
                                      const NARO::Vertex& v,
                                      bool verbose=false)
{
  double w_degree = 0.0L;
  auto neighbours = boost::adjacent_vertices(v, g);
  for (auto neighbor : boost::make_iterator_range(neighbours)) {
    auto edge = boost::edge(v, neighbor, g).first;
    w_degree += g[edge].distance;
  }

  return w_degree;
}
///  self loops
double NARO::get_nb_selfloops(NARO::Graph& g,
                              const NARO::Vertex& v,
                              bool verbose=false)
{
  double re = 0.0L;
  auto selfloop_edge = boost::edge(v, v, g);
  if (selfloop_edge.second){
    auto edge = selfloop_edge.first;
    auto dist = g[edge].distance;
    if (dist != -1 && dist >= 0){
      return dist;
    } else {
      return 1.0L;
    }
  }
  return re;
}
// -----------------------------------------------------------------------------
// bilab::get_filtered_map()
//
NARO::Filtered NARO::get_filtered_map(NARO::Graph& g, int cId, bool verbose)
{
  NARO::filter_communityId predicate{&g, cId};
  NARO::Filtered fg(g, predicate, predicate);
  if (verbose) {
    boost::write_graphviz(std::cout, fg,
                          boost::make_label_writer(
                          boost::get(&NARO::gVertex::name, fg)));
  }
  return fg;
}

// -----------------------------------------------------------------------------
// bilab::get_community_byId()
//
NARO::by_community_idx_t
NARO::get_community_byId (NARO::Graph& g, int cId, bool verbose)

{ // 1. define vertexMap
  NARO::by_community_idx_t commId_idx;
  auto reindex = [&] {
    commId_idx.clear();
    for (auto vd : boost::make_iterator_range(boost::vertices(g))) {
      if (g[vd].communityId == cId)
        commId_idx.insert(g[vd]);
    }
  };
  reindex();
  if (verbose)
    std::cout << "Index: " << commId_idx.size() << " elements\n";
  return commId_idx;
}

// -----------------------------------------------------------------------------
// bilab::get_community_byMap()
//
NARO::NameVertexMap NARO::get_community_byMap(NARO::Graph& g, int cId)
{
  NARO::NameVertexMap name2vertex;
  if (cId == -1) {
    for (auto vd : boost::make_iterator_range(boost::vertices(g))) {
      name2vertex.insert(std::make_pair(g[vd].name, vd));
    }
  } else {
    for (auto vd : boost::make_iterator_range(boost::vertices(g))) {
      if (g[vd].communityId == cId)
        name2vertex.insert(std::make_pair(g[vd].name, vd));
    }
  }
  return name2vertex;
}
// -----------------------------------------------------------------------------
// bilab::get_community_degree()
//
long double NARO::get_community_degree(NARO::Graph& g, int cId)
{
  long double degree = 0.0L;
  long double d = 0.0L;
  // get subgraph by their community ids
  NARO::Filtered fg = NARO::get_filtered_map(g, cId, false);
  auto es = boost::edges(fg);
  for (auto eit = es.first; eit != es.second; ++eit) {
    degree += fg[*eit].distance;
  }
  NARO::NameVertexMap name2vertex = get_community_byMap(g, cId);

  NARO::by_community_idx_t community_members;
  community_members = NARO::get_community_byId(g, cId, false);
  for (auto& vertex1 : community_members) {
    for(auto& vertex2: community_members) {
      if (vertex1 == vertex2) {
        continue;
      }else{
        NARO::Vertex v1 = name2vertex[vertex1.name];
        NARO::Vertex v2 = name2vertex[vertex2.name];
        auto edge = boost::edge(v1, v2, g);
        if (edge.second) {
          d += g[edge.first].distance;
        }
      }
    }
  }
  std::cout << "" << d << std::endl;
  return degree;
}


