//
//  graph_report.cpp
//  networkz
//
//  Created by 曹巍 on 2020/08/03.
//  Copyright © 2020 曹巍. All rights reserved.
//

#include <iostream>
#include <time.h>
#include <boost/timer/timer.hpp>

#include "graph_report.hpp"

// -----------------------------------------------------------------------------
// Write a report for graph analysis
// 1. Basic info about the graph (based on Boost Graph Library)
// 1.1 The number of nodes.
// 1.2 The number of edges.
//
std::string operator*(std::string str, std::size_t n)
{
    return repeat(std::move(str), n);
}

// -----------------------------------------------------------------------------
//
void markdown_writer(std::ofstream& out, const std::string& message) {
  out << message << "  \n";
}

// -----------------------------------------------------------------------------
//
void markdown_writer(std::ofstream& out, const std::string& message,
                     int header) {
  std::string head = "#";
  std::string u_score = "=";
  head += head * header;
  out << head + " " + message << "  \n";
}

// -----------------------------------------------------------------------------
//
bool NARO::Report::write(const std::string& o_filename, Graph* g,
                         const std::string& fmt, double threshold,
                         bool verbose=false) {
  std::chrono::duration<double> seconds;
  boost::timer::cpu_timer timer;
  std::ofstream ofile(o_filename);
  
  auto n_vertices = boost::num_vertices(*g);
  auto n_edges = boost::num_edges(*g);
  
  std::vector<int> component(n_vertices);
  int num = boost::connected_components(*g, &component[0]);
  
  if (verbose) {
    timer.start();
    std::cout << "Create a report for the graph ... ";
  }
  
  if (ofile.is_open()) {
    markdown_writer(ofile, "Basic info", 2);
    markdown_writer(ofile, "title : " + this->title);
    markdown_writer(ofile, "Author: " + this->author);
    markdown_writer(ofile, "Date  : " + this->date);
    markdown_writer(ofile, "\n");
    
    markdown_writer(ofile, "Network info", 2);
    markdown_writer(ofile, "Threshold for distance : " + std::to_string(
                                                            threshold));
    markdown_writer(ofile, "Nodes : " + std::to_string(n_vertices));
    markdown_writer(ofile, "Edges : " + std::to_string(n_edges));
    markdown_writer(ofile, "Connected components : " + std::to_string(num));
    markdown_writer(ofile, "\n");
    
    markdown_writer(ofile, "Vertex degree", 2);
    //auto vd = boost::vertices(*g);
    VertexIter vi, vend;
    for(boost::tie(vi, vend) = boost::vertices(*g); vi != vend; ++vi) {
      auto vertex_name = (*g)[*vi].name;
      auto degree = boost::degree(*vi, *g);
      auto component_num = component[*vi];
      markdown_writer(ofile,
                      vertex_name + ":" + std::to_string(component_num)
                      + ": " + std::to_string(degree));
    }
/*
    for (auto vd : boost::make_iterator_range(boost::vertices(*g))) {
      auto vertex_name = (*g)[vd].name;
      auto degree = boost::degree(vd, *g);
      markdown_writer(ofile, vertex_name + ": " + std::to_string(degree));
    }
*/
  }else{
    std::cerr << "Failed to write to "<< o_filename << std::endl;
  }
  ofile.close();
  if (verbose) {
    timer.stop();
    seconds = std::chrono::nanoseconds(timer.elapsed().user);
    std::cout << "Completed in " << seconds.count() << " seconds." << std::endl;
  }
  return true;
}

