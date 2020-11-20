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

#endif

#include "utility.hpp"

namespace NARO {
/**
 * @brief The vertice descriptor
 *
 *  The boost bundled properties.
 * @see [boost bundled properties](https://www.boost.org/doc/libs/1_72_0/libs/graph/doc/bundles.html).
 */
 struct gVertex {
   /// Constructor
   gVertex()
   {}
   /// Constructor with vertex name and weight
   gVertex(std::string& name, double weight = -1)
     : name(name), weight(weight)
   {}

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
     : glabel(glabel), total_weights(-1)
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
   std::string glabel; ///< Store the title for the graph
   double total_weights; ///< Total weights of edges
   double max_weights; ///< maximum of weights
   double min_weights; ///< minimum of weights
   int num_vertices; ///< The number of vertices
   int num_edges; ///< The number of edges
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
/// Defined type for vertex
typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;
/// Defined type for edge
typedef boost::graph_traits<Graph>::edge_descriptor Edge;
/// Get the type of vetex
typedef boost::vertex_bundle_type<Graph>::type VertexType;
/// Iterator for looping over vertices
typedef boost::graph_traits<Graph>::vertex_iterator VertexIter;
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
 * Graph-specific functions
 *
 */
/// @TODO hub score and authority score: Kleinberg (1999, 200)
std::tuple<double, double> get_hits_scores(NARO::Graph& g, NARO::Vertex& v);
/// Total weights
double get_total_weights(NARO::Graph& g, bool verbose);
/// weighted degree of a node
double get_node_weighted_degree(NARO::Graph& g, NARO::Vertex& v, bool verbose);
/// @TODO self loops
double get_nb_selfloops(NARO::Graph& g, NARO::Vertex& v, bool verbose);

} // End of namespace NARO


#endif /* graph_auxiliary_h */
