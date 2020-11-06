//
//  main.cpp
//  networkz
//
//  Created by 曹巍 on 2020/07/25.
//  Last Modified on 2020/07/29.
//  Copyright © 2020 曹巍. All rights reserved.
//
//
#include "common.hpp"
#include "cli_opt.hpp"
#include "utility.hpp"
#include "graph.hpp"
#include "graph_report.hpp"
#include "FileReaderFactory.hpp"
#include "TSVReader.hpp"
#include "graph_util.hpp"
#include "graph_algo.hpp"

int main(int argc, const char * argv[]) {
  // ---------------------------------------------------------------------------
  // Parse cmd line arguments
  // ---------------------------------------------------------------------------
  CLIARG::ParseCmdLine(argc, argv);
  // Parser name.
  std::string parser_name = CLIARG::parser_name;
  // log level
  bool verbose = CLIARG::verbose;
  // Input data file name.
  std::string filename = CLIARG::i_filename;
  // graphviz dot file name.
  std::string graphviz_file = CLIARG::o_graph_file;
  // graph description file name.
  std::string o_filename = CLIARG::o_filename;
  // graph title
  std::string o_graph_name = CLIARG::o_graph_name;
  // distance threshold to create an edge.
  double d_threshold = CLIARG::d_threshold;
  // method name for distance computation
  std::string distance_type = CLIARG::distance_type;
  // method name for finding the MST
  std::string mst_algo_name = CLIARG::mst_algo_name;
  
  // Options for file parser
  std::string sep = "\t";
  std::string comment = "#";
  int header = 0;
  
  // For measuring elapsed time
  std::chrono::duration<double> seconds;
  // manually start timer
  boost::timer::cpu_timer timer;
  // Prepare dataframe to store data.
  NARO::DataFrame *df = new NARO::DataFrame();
  if (df == 0x0) {
    std::cerr << "Insufficient memory" << std::endl;
    std::exit(-1);
  }
  // ---------------------------------------------------------------------------
  // Load the file parsers
  // ---------------------------------------------------------------------------
  if (verbose) {
    NARO::FileReaderFactory::showClasses();
  }
  auto reader = NARO::FileReaderFactory::makeUnique(parser_name);
  std::cout << " type of reader = " << reader->getType() << "\n";

  // ---------------------------------------------------------------------------
  // Show backend dependency
  // ---------------------------------------------------------------------------
  welcome_message();
  
#if defined(PARALLEL_BGL)
  boost::mpi::environment  env;
  boost::mpi::communicator comm;
#endif

  if (verbose) {
    timer.start();
  }
  // ---------------------------------------------------------------------------
  // 1. Read the tsv:
  // ---------------------------------------------------------------------------
  if (reader->read(filename, df, sep, comment, header)) {
    timer.stop();
    NARO::ErrorCode err;
    auto filesize = byteConverter_s(NARO::fs::file_size(filename, err));
    if (verbose) {
      seconds =
        std::chrono::nanoseconds(timer.elapsed().user);
      std::cout << "Read "<< filename << "(" << filesize  << ") completed."
                << " in " << seconds.count() << " seconds." << std::endl;
    } else {
      std::cout << "Read "<< filename << "(" << filesize  << ") completed."
                << std::endl;
    }
    df->head();
  } else {
    std::cerr << "Failed to read the input file, " << filename << std::endl;
    std::exit(-1);
  }
  // 1.1 extract data ->  target_id  tpm
  if (verbose) {
    std::cout << "Extract data ...";
    timer.start();
  } else {
    std::cout << "Extract data ...";
  }
  // ---------------------------------------------------------------------------
  // 1.1 Select columns
  // ---------------------------------------------------------------------------
  NARO::DataFrame *dat = new NARO::DataFrame();
  if (dat == 0x0) {
    std::cerr << "Insufficient memory" << std::endl;
    std::exit(-1);
  }
  if (CLIARG::column_names.empty()) {
    dat = df;
  } else {
    if(!df->select(CLIARG::column_names, dat)){
      std::cout<< "Failed to select the columns in the data file." << std::endl;
    }
  }
  if (verbose){
    timer.stop();
    seconds = std::chrono::nanoseconds(timer.elapsed().user);
    std::cout << " Completed in " << seconds.count() << " seconds."
              << std::endl;
    dat->describe();
  } else {
    std::cout << " Completed." << std::endl;
  }
  // ---------------------------------------------------------------------------
  // 2. Compute the cityblock distance between any two genes
  // ---------------------------------------------------------------------------
  // Instantiate a graph
  if (verbose) {
    std::cout << "Create a graph ... ";
    timer.start();
  } else {
    std::cout << "Create a graph ... ";
  }
  
  NARO::Graph genes_graph(NARO::gGraph{CLIARG::o_graph_name});
  if(!NARO::create_graph(&genes_graph, dat, d_threshold, distance_type)){
    std::cerr << "Failed to create the graph" << std::endl;
  }
  
  if (verbose) {
    timer.stop();
    seconds = std::chrono::nanoseconds(timer.elapsed().user);
    std::cout << " Completed in " << seconds.count() << " seconds." << std::endl;
  } else {
    std::cout << " Completed." << std::endl;
  }
  // ---------------------------------------------------------------------------
  // Find the minimum spanning tree by prim (more edges)
  //     or kruskal (more vertices)
  //
  genes_graph.m_property->glabel += "-" + mst_algo_name;
  NARO::Algo::find_minimum_spanning_tree(
                      genes_graph, mst_algo_name);
  // ---------------------------------------------------------------------------
  // Find the minimum cut by stoer_wagner_min_cut
  NARO::Algo::stoer_wagner_min_cut(genes_graph, true);
  // ---------------------------------------------------------------------------
  // Create a report
  //
  NARO::Report report{o_graph_name, get_local_time(), "Networkz"};
  if (!report.write(o_filename, &genes_graph, "md", d_threshold, verbose)){
    std::cout << "Failed to write a log." <<std::endl;
    std::exit(-1);
  }

  // Write to graphviz format if requested.
  if (!graphviz_file.empty()) {
    std::cout << "Write to a " << graphviz_file << " ... ";
    if (verbose) {
      timer.start();
    }
    NARO::write_to_graphviz(graphviz_file, genes_graph);
    if (verbose) {
      timer.stop();
      seconds = std::chrono::nanoseconds(timer.elapsed().user);
      std::cout << " completed in " << seconds.count() << " seconds."
                << std::endl;
    }else{
      std::cout << " completed." << std::endl;
    }
  }
  if (verbose) {
    std::cout << "Clean memory ... ";
    timer.start();
  } else {
    std::cout << "Clean memory ... ";
  }
  // ---------------------------------------------------------------------------
  // Release Memory
  // ---------------------------------------------------------------------------
  genes_graph.clear();
  // dat and df are the same object
  if (dat == df) {
    dat = nullptr;
    delete df;
  } else {
    delete dat;
    delete df;
  }
  if (verbose) {
    timer.stop();
    seconds = std::chrono::nanoseconds(timer.elapsed().user);
    std::cout << "Completed "
              << "in " << seconds.count() << " seconds." << std::endl;
  } else {
    std::cout << "Completed." << std::endl;
  }
  
  std::cout << "Program Exit." << std::endl;
  return EXIT_SUCCESS;
}
