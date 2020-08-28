//
//  main.cpp
//  networkz
//
//  Created by 曹巍 on 2020/07/25.
//  Last Modified on 2020/07/29.
//  Copyright © 2020 曹巍. All rights reserved.
//
//
/* -----------------------------------------------------------------------------
 * Compilation:
 * clang++ -Os -std=c++17 main.cpp -o networkz \
 *      -I/Users/davecao/dist/eigen/include/eigen3 \
 *      -I/Users/davecao/dist/boost/include \
 *      -L/Users/davecao/dist/boost/lib \
 *      -lboost_program_options -lboost_iostreams -lboost_timer -lboost_graph \
 *      -lc++
 *
 * Usage:
 *  General options:
 *  -V [ --version ]        Show the version number
 *  -i [ --infile ] arg     The input data file. Only the tsv format is supported
 *                          now.
 *  -o [ --outfile ] arg    The output file of clusters in text format.Default
 *                          name is 'o_graphviz.gv'.
 *  -c [ --column ] arg     The column name in the input file. Default is 'tpm'
 *  -g [ --graph ] arg      Output the graph to a file in the graphviz format if
 *                          true.
 *  -h [ --help ]           print help info.
 *
 * Graph options:
 *  -v [ --verbose ]        The extra verbose.
 *  -t [ --threshold ] arg  The threshold between two vertices which are linked
 *                          by an edge if the distance less than it. default is
 *                          0.
 * -n [ --title ] arg      The title of the graph used to label the png file.
 *                         Default is Gene Expression Network.
 *
 * Example:
 *  1. Create a graphviz file.
 *  $> ./networkz -i /path/to/your/abundance.tsv -t 2.0 -o abundance.gv
 *  2. Generate a png by `dot` command in Graphviz package.
 *  $> dot -Tpng o_graphviz.gv -o o_graphviz.png
 * ---------------------------------------------------------------------------*/

#include "common.hpp"
#include "cli_opt.hpp"
#include "utility.hpp"
#include "graph.hpp"
#include "graph_report.hpp"
#include "FileReaderFactory.hpp"
#include "TSVReader.hpp"
#include "graph_util.hpp"

int main(int argc, const char * argv[]) {

  //namespace fs = std::filesystem;
  NARO::FileReaderFactory::showClasses();
  //std::unique_ptr<NARO::FileReaderBase> reader =
  auto reader = NARO::FileReaderFactory::makeUnique("TSVReader");

  std::cout << " type of reader = " << reader->getType() << "\n";
  //NARO::FileReaderBase* reader = new NARO::TSVReader;
  std::chrono::duration<double> seconds;

  // Parse cmd line arguments
  CLIARG::ParseCmdLine(argc, argv);
  std::string filename = CLIARG::i_filename;
  std::string graphviz_file = CLIARG::o_graph_file;
  
  std::string sep = "\t";
  std::string comment = "#";
  int header = 0;
  
#if defined(PARALLEL_BGL)
  boost::mpi::environment  env;
  boost::mpi::communicator comm;
#endif
  
  NARO::DataFrame *df = new NARO::DataFrame();
  // Show backend dependency
  welcome_message();
  
  // manually start timer
  boost::timer::cpu_timer timer;
  if (CLIARG::verbose) {
    timer.start();
  }
  // ---------------------------------------------------------------------------
  // 1. Read the tsv:
  // ---------------------------------------------------------------------------
  if (reader->read(filename, df, sep, comment, header)) {
    timer.stop();
    //auto err = std::error_code{};
    NARO::ErrorCode err;
    auto filesize = byteConverter_s(NARO::fs::file_size(filename, err));
    if (CLIARG::verbose) {
      seconds =
        std::chrono::nanoseconds(timer.elapsed().user);
      std::cout << "Read "<< filename << "(" << filesize  << ") completed."
                << " in " << seconds.count() << " seconds." << std::endl;
    } else {
      std::cout << "Read "<< filename << "(" << filesize  << ") completed."
                << std::endl;
    }
  } else {
    std::cerr << "Failed to read the input file, " << filename << std::endl;
    std::exit(-1);
  }
  // 1.1 extract data ->  target_id  tpm
  if (CLIARG::verbose) {
    std::cout << "Extract data ...";
    timer.start();
  } else {
    std::cout << "Extract data ...";
  }

  NARO::DataFrame *dat = new NARO::DataFrame();

  if (CLIARG::verbose){
    timer.stop();
    seconds = std::chrono::nanoseconds(timer.elapsed().user);
    std::cout << " Completed in " << seconds.count() << " seconds." << std::endl;
  } else {
    std::cout << " Completed." << std::endl;
  }
  // ---------------------------------------------------------------------------
  // 2. Compute the cityblock distance between any two genes
  // ---------------------------------------------------------------------------
  // Instantiate a graph
  if (CLIARG::verbose) {
    std::cout << "Create a graph ... ";
    timer.start();
  } else {
    std::cout << "Create a graph ... ";
  }
  
  NARO::Graph genes_graph(NARO::gGraph{CLIARG::o_graph_name});
  if (CLIARG::distance_type == "city") {
    NARO::CityBlock dist;
    if (!NARO::create_graph(&genes_graph,
                                             dat,
                                             CLIARG::d_threshold,
                                             dist)) {
      std::cerr << "Failed to create the graph" << std::endl;
    }
  }else if (CLIARG::distance_type == "euc") {
    NARO::Euclidean dist;
    if (!NARO::create_graph(&genes_graph,
                                             dat,
                                             CLIARG::d_threshold,
                                             dist)) {
      std::cerr << "Failed to create the graph" << std::endl;
    }
  } else {
    NARO::Corrcoef dist;
    if (!NARO::create_graph(&genes_graph,
                                             dat,
                                             CLIARG::d_threshold,
                                             dist)) {
      std::cerr << "Failed to create the graph" << std::endl;
    }
  }
  if (CLIARG::verbose) {
    timer.stop();
    seconds = std::chrono::nanoseconds(timer.elapsed().user);
    std::cout << " Completed in " << seconds.count() << " seconds." << std::endl;
  } else {
    std::cout << " Completed." << std::endl;
  }
  // ---------------------------------------------------------------------------
  // Create a report
  //
  NARO::Report report{CLIARG::o_graph_name, get_local_time(), "Networkz"};
  if (!report.write(CLIARG::o_filename, &genes_graph, "md",
                    CLIARG::d_threshold,
                    CLIARG::verbose)){
    std::cout << "Failed to write a log." <<std::endl;
    std::exit(-1);
  }

  // Write to graphviz format if requested.
  if (!graphviz_file.empty()) {
    std::cout << "Write to a " << graphviz_file << " ... ";
    if (CLIARG::verbose) {
      timer.start();
    }
    // Write the graph to the output file
    std::ofstream graphfile(graphviz_file);
    boost::write_graphviz(graphfile, genes_graph,
      /* Vertex */
      [&] (auto& out, auto v) {
        out << genes_graph[v].to_graphviz();},
      /* Edge */
      [&] (auto& out, auto e) {
      out << genes_graph[e].to_graphviz();},
      /* Graph property */
      [&] (auto& out) {out<< genes_graph.m_property->to_graphviz();}
    );
    graphfile.close();
    if (CLIARG::verbose) {
      timer.stop();
      seconds = std::chrono::nanoseconds(timer.elapsed().user);
      std::cout << " completed in " << seconds.count() << " seconds."
                << std::endl;
    }else{
      std::cout << " completed." << std::endl;
    }
  }
  if (CLIARG::verbose) {
    std::cout << "Clean memory ... ";
    timer.start();
  } else {
    std::cout << "Clean memory ... ";
  }
  // Release Memory
  genes_graph.clear();
  delete df;
  delete dat;
  
  if (CLIARG::verbose) {
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
