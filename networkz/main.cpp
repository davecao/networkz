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
#include "community_detection.hpp"
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
  // graph component as text
  std::string comp_filename = "networkz_components.txt";
  // graph title
  const std::string o_graph_name = CLIARG::o_graph_name;
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
  
  // Precision
  long double precision = 0.000001L;
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
    //df->head();
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
    //dat->describe();
  } else {
    std::cout << " Completed." << std::endl;
  }
  // ---------------------------------------------------------------------------
  // 2. Compute the distance between any two genes
  // ---------------------------------------------------------------------------
  // Instantiate a graph
  if (verbose) {
    std::cout << "Start to create a graph " <<std::endl;
    timer.start();
  } else {
    std::cout << "Start to create a graph " << std::endl;
  }
  
  NARO::Graph genes_graph(NARO::gGraph{CLIARG::o_graph_name});
  if(!NARO::create_graph(&genes_graph, dat, d_threshold, distance_type)){
    std::cerr << "Failed to create the graph" << std::endl;
  }
  
  if (verbose) {
    timer.stop();
    seconds = std::chrono::nanoseconds(timer.elapsed().user);
    std::cout << "Completed in " << seconds.count() << " seconds." << std::endl;
  } else {
    std::cout << "Completed." << std::endl;
  }
  std::cout << "Number of vertices: "<< boost::num_vertices(genes_graph) << std::endl;
  std::cout << "Number of edges: " <<  boost::num_edges(genes_graph) << std::endl;
  // ---------------------------------------------------------------------------
  // Find the minimum spanning tree by prim (more edges)
  //     or kruskal (more vertices)
  //
  if (verbose) {
    std::cout << "\nFind minimum spanning tree ... ";
    timer.start();
  } else {
    std::cout << "\nFind minimum spanning tree ";
  }
  genes_graph.m_property->glabel += "-" + mst_algo_name;
  NARO::Algo::find_minimum_spanning_tree(
                      genes_graph, mst_algo_name);
  if (verbose) {
    timer.stop();
    seconds = std::chrono::nanoseconds(timer.elapsed().user);
    std::cout << " Completed in " << seconds.count() << " seconds." << std::endl;
  } else {
    std::cout << " Completed." << std::endl;
  }
  // ---------------------------------------------------------------------------
  // modularity::Modularity
  if (verbose) {
    std::cout << " Detect community ... ";
    timer.start();
  } else {
    std::cout << " Dectect community ";
  }
  std::map<std::string, unsigned int> n2str_table;
  //    1. Convert the graph to csr_graph
  NARO::Algo::Community::CSRgraph csr_g;
  NARO::convert(genes_graph, csr_g, &n2str_table);
  //    2. Initialize the quality function
  NARO::Algo::Community::Modularity quality(csr_g);
  //    3. Initilaize the louvain
  NARO::Algo::Community::Louvain<NARO::Algo::Community::Modularity>
      louvain(-1, precision, &quality);
  //    4. Execute louvain
  //       Store community ids with indices of nodes
  std::vector<int> n2c;
  double final_quality = -100;
  int num_levels = -1;

  std::tie(final_quality, num_levels) = louvain.louvain(n2c);
  std::cout << "Best quality: " << final_quality << std::endl;
  genes_graph[boost::graph_bundle].level = num_levels;
  genes_graph[boost::graph_bundle].quality = final_quality;
  genes_graph[boost::graph_bundle].quality_name = quality.name;
  //    5. Assign community id
  NARO::VertexIter vi, vend;
  for(boost::tie(vi, vend) = boost::vertices(genes_graph); vi != vend; ++vi) {
    auto vertex_name = genes_graph[*vi].name;
    auto node = n2str_table[vertex_name];
    auto communityId = n2c[node];
    genes_graph[*vi].communityId = communityId;
  }
  if (verbose) {
    timer.stop();
    seconds = std::chrono::nanoseconds(timer.elapsed().user);
    std::cout << " Completed in " << seconds.count() << " seconds." << std::endl;
  } else {
    std::cout << " Completed." << std::endl;
  }
  // ---------------------------------------------------------------------------
  // Create a report
  //
  if (verbose) {
    std::cout << " Create a report ... " <<std::endl;
    timer.start();
  } else {
    std::cout << " Create a report  " << std::endl;
  }
  NARO::Report report{o_graph_name, get_local_time(), "Networkz", filename};
  if (!report.write(o_filename, &genes_graph, "md", d_threshold,
                    distance_type, mst_algo_name,
                    verbose)){
    std::cout << "Failed to write a log." <<std::endl;
    std::exit(-1);
  }
  if (verbose) {
    timer.stop();
    seconds = std::chrono::nanoseconds(timer.elapsed().user);
    std::cout << " Completed in " << seconds.count() << " seconds." << std::endl;
  } else {
    std::cout << " Completed." << std::endl;
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
  // ---------------------------------------------------------------------------
  // Release Memory
  // ---------------------------------------------------------------------------
  if (verbose) {
    std::cout << "Clean memory ... ";
    timer.start();
  } else {
    std::cout << "Clean memory ... ";
  }
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
