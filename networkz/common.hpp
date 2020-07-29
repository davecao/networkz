//
//  common.h
//  networkz
//
//  Created by 曹巍 on 2020/07/25.
//  Copyright © 2020 曹巍. All rights reserved.
//

#ifndef common_h
#define common_h

// all the system #include's we'll ever need
#include <fstream>
#include <cmath>
#include <complex>
#include <iostream>
#include <iomanip>
#include <vector>
#include <list>
#include <map>
#include <stack>
#include <queue>
#include <algorithm>
#include <iterator>
#include <limits>   // numeric_limits
#include <typeinfo> // typeid
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <fcntl.h>
#include <string>
#include <ctype.h>
#include <assert.h>
#include <filesystem>
//#include <regex> // Not supported by clang++ with llvm 4.2.1

//Boost
#include <boost/config.hpp> // put this first to suppress some VC++ warnings
#include <boost/program_options.hpp>
#include <boost/thread.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/timer/timer.hpp>
#include <boost/format.hpp>
#include <boost/bind.hpp>
#include <boost/pending/stringtok.hpp>
#include <boost/regex.hpp>
#include <boost/any.hpp>
#include <boost/iostreams/device/mapped_file.hpp>
//#include <boost/xpressive/xpressive.hpp>

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

#endif

inline std::string byteConverter(long long byte)
{
  std::stringstream ss;
  if (byte < 1024){
    ss << byte <<" Bytes";
  }else if (byte <1048576 && byte >= 1024){
    ss << static_cast<float>(byte/1024) <<" KB";
  }else if (byte < 1073741824 && byte >= 1048576) {
    ss << static_cast<float>(byte/1048576) <<" MB";
  }else{
    ss << static_cast<float>(byte/1073741824)<<" GB";
  }
  return ss.str();
}

inline std::string byteConverter_s(long long byte)
{
  std::stringstream ss;
  if (byte < 1000){
    ss << byte <<" Bytes";
  }else if (byte <1000000 && byte >= 1000){
    ss << static_cast<float>(byte)/1000 <<" KB";
  }else if (byte < 1000000000 && byte >= 1000000) {
    ss << static_cast<float>(byte)/1000000 <<" MB";
  }else{
    ss << static_cast<float>(byte)/1000000000<<" GB";
  }
  return ss.str();
}

inline std::string get_local_time() {
  std::time_t t = std::time(nullptr);
  std::tm* now = std::localtime(&t);
  std::stringstream ss;
  ss << (now->tm_year + 1900) << "-"
     << (now->tm_mon + 1) << "-"
  << now->tm_mday;
  return ss.str();
}
/*
typedef std::tuple<std::string, std::string> TupleKey;

// define a hash function for this tuple
struct KeyHash : public std::unary_function<TupleKey, std::size_t>
{
  std::size_t operator()(const TupleKey& k) const {
    // the magic operation below makes collisions less likely than
    // just the standard XOR
    std::size_t seed = std::hash<std::string>()(std::get<0>(k));
    seed ^= std::hash<std::string>()(std::get<1>(k)) + 0x9e3779b9 +
            (seed << 6) + (seed >> 2);
    return seed;
  }
};

// define the comparison operator for this tuple
struct KeyEqual : public std::binary_function<TupleKey, TupleKey, bool>
{
  bool operator()(const TupleKey& v0, const TupleKey& v1) const {
    return (std::get<0>(v0) == std::get<0>(v1) &&
                std::get<1>(v0) == std::get<1>(v1));
  }
};
// Usage: std::map use tuple as the key
//
// typedef std::unordered_map<TupleKey, double, KeyHash, KeyEqual> EdgesMap;
// EdgesMap *edge_map = new EdgesMap();
// edge_map->insert(
//     std::pair<TupleKey, double>(
//         std::tuple<std::string, std::string>(n1, n2), d));
*/
std::vector<std::string> readFileToLines(const std::string& file,
                                         const std::string& comment="#")
{
  namespace fs = std::filesystem;
  std::vector<std::string> lines;
  // 1. Check the existance of the tsv file
  if (!fs::exists(file) && fs::is_regular_file(file))
  {
    std::cout << "Could not find the input file: "<< file << std::endl;
  }
  // 2. Open the file by Stream
  std::ifstream f(file);
  if (f) {
    std::string str;
    // Read the next line from File untill it reaches the end.
    while (std::getline(f, str))
    {
      // Remove the newline
      str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
      // Remove the comment lines
      if (str.substr(0, comment.length()) == comment){
        continue;
      }
      // Line contains string of length > 0 then save it in vector
      if(str.size() > 0)
          lines.push_back(str);
    }
  } else {
    std::cout << "Failed to open the file, "<< file << "." <<std::endl;
  }
  // 3. close
  f.close();
  return lines;
}
#endif /* common_h */
