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

 typedef Eigen::MatrixXd Dynamic2D;

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

 // Data structure for CSV format based on Eigen::MatrixXd
 struct Index
 {
   std::map<std::string, size_t> Names;
   std::vector<std::pair<std::string, size_t>> OrderedNames;
   bool sorted = false;

   ssize_t GetIndex(std::string& name) const;
   std::vector<std::string> GetIndexNames();

 private:
   void SortIndex();
 };

 struct DataFrame
 {
   EIGEN_MAKE_ALIGNED_OPERATOR_NEW
   size_t num_rows = 0;
   size_t num_cols = 0;

   Index *rowIndex;
   Index *columnIndex;
   
   Dynamic2D data;

   DataFrame();
   DataFrame(size_t nrows, size_t ncols);
   ~DataFrame();

   void resize(size_t nrows, size_t ncols);
   size_t size();
   void set_data(Dynamic2D& d);
   double operator()(int r_inx, int c_inx);

   bool set_columnIndex_names(std::string& col_name, size_t i);
   bool set_columnIndex_names(std::vector<std::string>& col_name);
   std::vector<std::string> get_columnIndex_names();
   bool set_rowIndex_names(std::vector<std::string>& row_name);
   std::vector<std::string> get_rowIndex_names();
   bool add_row(std::vector<std::string>& row, size_t rInx);
   /*
    * Select data by a column name and return a copy of sub-dataset.
    *  time-consuming and may cause the memory leaks
    */
   DataFrame select(std::string& c_name);
   /*
    * Modify the passed dataframe and store the sub dataset.
    * Release the memory from the main function to avoid the memory leaks.
    */
   bool select(std::string& c_name, DataFrame* sliced);
 };
 
} // End of namespace NARO


#endif /* graph_auxiliary_h */
