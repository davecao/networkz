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

struct Report
{
  // Fields
  std::string title;
  std::string date;
  std::string author;

  bool write(const std::string& o_filename, Graph* g,
             const std::string& fmt, double threshold,
             bool verbose);

};

}

#endif /* graph_report_hpp */
