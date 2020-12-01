//
//  graph.hpp
//  networkz
//
//  Created by CAO Wei on 2020/07/30.
//  Copyright © 2020 曹巍. All rights reserved.
//

#ifndef graph_auxiliary_h
#define graph_auxiliary_h

#include <Eigen/Core>
#include <vector>
#include <map>
#include <assert.h>

#if defined(PARALLEL_BGL)
// Parallel BGL
#include <mpi.h>
#include <boost/mpi.hpp>
#include <boost/graph/use_mpi.hpp>
#include <boost/graph/distributed/mpi_process_group.hpp>
#include <boost/graph/distributed/adjacency_list.hpp>
#include <boost/graph/erdos_renyi_generator.hpp>
#include <boost/random/linear_congruential.hpp>

#else

//Boost Graph Libraries
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/adjacency_matrix.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/exterior_property.hpp>
#include <boost/graph/topological_sort.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/clustering_coefficient.hpp>
#include <boost/property_map/dynamic_property_map.hpp>
#include <boost/graph/visitors.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/bc_clustering.hpp>
#include <boost/graph/kruskal_min_spanning_tree.hpp>
#include <boost/graph/prim_minimum_spanning_tree.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/graph/incremental_components.hpp>
#include <boost/pending/disjoint_sets.hpp>
#include <boost/graph/filtered_graph.hpp>
#include <boost/intrusive/set_hook.hpp>
#include <boost/intrusive/set.hpp>

#endif

#include "utility.hpp"

namespace bi = boost::intrusive;

namespace NARO {

/**
 * @brief The vertice descriptor
 *
 *  The boost bundled properties.
 * @see [boost bundled properties](https://www.boost.org/doc/libs/1_72_0/libs/graph/doc/bundles.html).
 */
 struct gVertex
  : bi::set_base_hook<bi::link_mode<bi::auto_unlink>,
                      bi::constant_time_size<false>>
 {
   /// Constructor
   gVertex()
   {}
   /// Constructor with vertex name and weight
   gVertex(std::string& name, double weight = -1)
     : name(name), weight(weight)
   {}
    
   friend bool operator==(const gVertex& lhs, const gVertex& rhs);
 
   struct by_commId
   {
     using type = long double;
     type operator()(gVertex const& vd) const
     {
       return vd.communityId;
     }
   };
 #if defined(PARALLEL_BGL)
   // Serialization support for parallel processing
   template<typename Archiver>
   void serialize(Archiver& ar, const unsigned int /*version*/) {
     ar & name & weight;
   }
 #endif
   /**
    *  To generate the current vertice info in graphviz format
    */
   std::string to_graphviz();
   
   std::string name; ///< Vertice name
   double weight; ///< Verice weight
   int communityId; ///< Community id
   int componentId; ///< Component id
 };
 
/**
 * @brief The edge descriptor
 *
 *  The boost bundled properties.
 *  @see [boost bundled properties](https://www.boost.org/doc/libs/1_72_0/libs/graph/doc/bundles.html).
*/
 struct gEdge {
   /// Constructor
   gEdge()
   {}
   /// Constructor with the default distance as -1
   gEdge(double distance=-1)
     : distance(distance)
   {}
 
 #if defined(PARALLEL_BGL)
   // Serialization support for parallel processing
   template<typename Archiver>
   void serialize(Archiver& ar, const unsigned int /*version*/) {
     ar & distance;
   }
 #endif
   /**
    * To generate the current edge info in graphviz format
    */
   std::string to_graphviz();
   
   double distance; ///< distance  between two vertices, i.e., edge weight
 };
/**
 * @brief The graph descriptor
 *
 *  The boost bundled properties.
 *  @see [boost bundled properties](https://www.boost.org/doc/libs/1_72_0/libs/graph/doc/bundles.html).
 */
 struct gGraph {
   /// Constructor
   gGraph()
   {}
   /// Constructor with a string label as title
   gGraph(const std::string& glabel)
     : glabel(glabel), total_weights(-1), level(0)
   {}
   
 #if defined(PARALLEL_BGL)
   // Serialization support for parallel processing
   template<typename Archiver>
   void serialize(Archiver& ar, const unsigned int /*version*/) {
     ar & glabel;
   }
 #endif
   /**
    * To generate the current edge info in graphviz format
    */
   std::string to_graphviz();
//   std::map<std::string, Vertex> name2v;
   std::string glabel; ///< Store the title for the graph
   double total_weights; ///< Total weights of edges
   double max_weights; ///< maximum of weights
   double min_weights; ///< minimum of weights
   int num_vertices; ///< The number of vertices
   int num_edges; ///< The number of edges
   int level; ///< Louvain's level
   double quality; ///< Louvain's quality
   std::string quality_name; ///< Name of quality function
 };

 #if defined(PARALLEL_BGL)
 // Use parallel BGL
 typedef boost::adjacency_list<
   boost::vecS,
   boost::distributedS<boost::graph::distributed::mpi_process_group,
                       boost::vecS>,
   boost::undirectedS,
   gVertex, ///< bundled property for vertex
   gEdge,   ///< bundled property for edge
   gGraph   ///< bundled property for graph
 > Graph;
 #else
 typedef boost::adjacency_list<
   boost::listS, ///< edge container type: list
   boost::vecS,  ///< vertex container type: vector
   boost::undirectedS, ///< Graph type: undirected graph
   gVertex, ///< bundled property for vertex
   gEdge,   ///< bundled property for edge
   gGraph   ///< bundled property for graph
 > Graph;
 #endif

/**
 * Graph-specific definitions
 *
 */
/// Define  vertex
typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;
/// Get the type of vetex
typedef boost::vertex_bundle_type<Graph>::type VertexType;
/// Iterator for looping over vertices
typedef boost::graph_traits<Graph>::vertex_iterator VertexIter;
/// Vertex index
typedef boost::graph_traits<Graph>::vertices_size_type VertexIndex;

typedef VertexIndex* Rank;
typedef Vertex* Parent;

/**
 * @brief Connected components:
 *
 * Using vecS for the graph type, we're effectively using identity_property_map for a vertex index map.
 * Using listS instead, the index map would need to be explicitly passed to the component_index constructor.
 */
typedef boost::component_index<VertexIndex> Components;

/// Defined type for edge
typedef boost::graph_traits<Graph>::edge_descriptor Edge;
/// Iterator for looping over edges
typedef boost::graph_traits<Graph>::edge_iterator EdgeIter;

// abstract accessor for the clustering coefficients of vertices
/// clustering property
typedef boost::exterior_vertex_property<Graph, float> ClusteringProperty;
/// clustering container
typedef ClusteringProperty::container_type ClusteringContainer;
/// clustering map
typedef ClusteringProperty::map_type ClusteringMap;
/// coefficients
//typedef typename boost::property_traits<ClusteringMap>::value_type Coefficient;
/// Container for testing the existence of a vertex in the graph
typedef std::map<std::string, Vertex> NameVertexMap;

/**
 * @brief filtered graph
 *
 *  It is a predicate function object.
 * Note: The fields of a vertex, communityId, must be assigned first.
 */
struct filter_communityId
{
  filter_communityId()
    : g_pt(nullptr), comm_id(0)
  {}
  
  filter_communityId(NARO::Graph* g, int cId)
  {
    g_pt = g;
    comm_id = cId;
  }
  
  bool operator()(NARO::Edge e) const {
    // all edge
    return true;
  }
  
  bool operator()(NARO::Vertex v) const {
    return (*g_pt)[v].communityId == comm_id;
  }

  NARO::Graph* g_pt;
  int comm_id;
};

using Filtered = boost::filtered_graph<Graph,
                                      filter_communityId,
                                      filter_communityId>;

using by_community_idx_t = bi::set<gVertex,
                                   bi::constant_time_size<false>,
                                   bi::key_of_value<gVertex::by_commId> >;

/**
 * Graph-specific functions
 *
 */
NameVertexMap get_community_byMap(NARO::Graph& g, int cId);

Filtered get_filtered_map(NARO::Graph& g, int cId, bool verbose);

by_community_idx_t
get_community_byId (NARO::Graph& g, int cId, bool verbose);

/// @TODO hub score and authority score: Kleinberg (1999, 200)
std::tuple<double, double> get_hits_scores(NARO::Graph& g, NARO::Vertex& v);
/// Total weights
double get_total_weights(NARO::Graph& g, bool verbose);
/// weighted degree of a node
double get_node_weighted_degree(NARO::Graph& g,
                                const NARO::Vertex& v,
                                bool verbose);
/**
 * @brief Get selfloop for the node
 */
double get_nb_selfloops(NARO::Graph& g,
                        const NARO::Vertex& v,
                        bool verbose);

long double get_community_degree(NARO::Graph& g, int cId);

} // End of namespace NARO


#endif /* graph_auxiliary_h */
