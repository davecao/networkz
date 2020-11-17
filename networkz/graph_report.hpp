//
//  graph_report.hpp
//  networkz
//
//  Created by 曹巍 on 2020/08/03.
//  Copyright © 2020 曹巍. All rights reserved.
//

#ifndef graph_report_hpp
#define graph_report_hpp

#include "graph.hpp"
#include "utility.hpp"

namespace NARO {

/**
 * @brief Generate a simple report for a given graph to a file.
 */
struct Report
{
  /// Report title
  std::string title;
  /// The created date for output
  std::string date;
  /// Author name
  std::string author;

  /**
   * @brief Generate a simple report for a given graph to a file.
   *
   * @param[in] o_filename the path of the output filename
   * @param[in] g the constructed graph
   * @param[in] fmt the output format (Not implemented yet)
   * @param[in] threshold the upper bound for the distance to create an edge between any two vertices
   * @param[in] distanceType the name for distance caculation
   * @param[in] verbose bool, true to print more info
   */
  bool write(const std::string& o_filename, Graph* g,
             const std::string& fmt, double threshold,
             const std::string& distanceType,
             bool verbose);

};

}

#endif /* graph_report_hpp */
