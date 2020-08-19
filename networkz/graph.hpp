//
//  graph_auxiliary.hpp
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

 struct gVertex {
   // Constructor
   gVertex()
   {}
   
   gVertex(std::string& name, double tpm = -1)
     : name(name), tpm(tpm)
   {}
   // public fields
   std::string name;
   double tpm;
 #if defined(PARALLEL_BGL)
   // Serialization support for parallel processing
   template<typename Archiver>
   void serialize(Archiver& ar, const unsigned int /*version*/) {
     ar & name & tpm;
   }
 #endif
   std::string to_graphviz();
 };
 
 struct gEdge {
   // Constructor
   gEdge()
   {}
   
   gEdge(double distance=-1)
     : distance(distance)
   {}
   
   double distance;

 #if defined(PARALLEL_BGL)
   // Serialization support for parallel processing
   template<typename Archiver>
   void serialize(Archiver& ar, const unsigned int /*version*/) {
     ar & distance;
   }
 #endif
   std::string to_graphviz();
 };

 struct gGraph {
   //Constructor
   gGraph()
   {}
   
   gGraph(const std::string& glabel)
     : glabel(glabel)
   {}
   
   std::string glabel;

 #if defined(PARALLEL_BGL)
   // Serialization support for parallel processing
   template<typename Archiver>
   void serialize(Archiver& ar, const unsigned int /*version*/) {
     ar & glabel;
   }
 #endif
   std::string to_graphviz();
 };

 #if defined(PARALLEL_BGL)
 // Use parallel BGL
 typedef boost::adjacency_list<
   boost::vecS,
   boost::distributedS<boost::graph::distributed::mpi_process_group, boost::vecS>,
   boost::undirectedS,
   gVertex, // bundled property for vertex
   gEdge,   // bundled property for edge
   gGraph   // bundled property for graph
 > Graph;
 #else
 typedef boost::adjacency_list<
   boost::listS, // edge container type: list
   boost::vecS,  // vertex container type: list
   boost::undirectedS, // Graph type: undirected graph
   gVertex, // bundled property for vertex
   gEdge,   // bundled property for edge
   gGraph   // bundled property for graph
 > Graph;

 #endif

 // Graph-specific definitions
 typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;
 typedef boost::graph_traits<Graph>::edge_descriptor Edge;
 typedef boost::vertex_bundle_type<Graph>::type VertexType;
 typedef boost::graph_traits<Graph>::vertex_iterator VertexIter;
 // Container for testing the existence of a vertex in the graph
 typedef std::map<std::string, Vertex> NameVertexMap;
 
} // End of namespace NARO


#endif /* graph_auxiliary_h */
