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

#endif /* common_h */
