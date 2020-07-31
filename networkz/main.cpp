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
#include "graph_auxiliary.hpp"
#include "file_reader_factory.hpp"


int main(int argc, const char * argv[]) {
  
  namespace fs = std::filesystem;

  std::chrono::duration<double> seconds;

  // Parse cmd line arguments
  CLIARG::ParseCmdLine(argc, argv);
  std::string filename = CLIARG::i_filename;
  std::string graphviz_file = CLIARG::o_filename;
  
  std::string sep = "\t";
  std::string comment = "#";
  std::string col = CLIARG::column_name;
  int header = 0;
  
#if defined(PARALLEL_BGL)
  boost::mpi::environment  env;
  boost::mpi::communicator comm;
#endif
  
  bilab::DataFrame *df = new bilab::DataFrame();
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
  if (read_tsv(filename, df, sep, comment, header)) {
    timer.stop();
    auto err = std::error_code{};
    auto filesize = byteConverter_s(fs::file_size(filename, err));
    if (CLIARG::verbose) {
      seconds =
        std::chrono::nanoseconds(timer.elapsed().user);
      std::cout << "Read "<< filename << "(" << filesize  << ") completed."
                << " in " << seconds.count() << " seconds." << std::endl;
    } else {
      std::cout << "Read "<< filename << "(" << filesize  << ") completed."
                << std::endl;
    }
  }
  // 1.1 extract data ->  target_id  tpm
  if (CLIARG::verbose) {
    std::cout << "Extract data ...";
    timer.start();
  } else {
    std::cout << "Extract data ...";
  }
  
  auto rownames = df->get_rowIndex_names();
  //auto dat = df->select(col);
  bilab::DataFrame *dat = new bilab::DataFrame();
  if (!df->select(col, dat)) {
    std::cout<< col << " Not found." << std::endl;
    std::exit(-1);
  }
  std::vector<int> sel_inx;
  for (int i=0; i<dat->size(); i++) {
    if ((*dat)(i, 0) != 0) {
      sel_inx.push_back(i);
    }
  }
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
  
  bilab::Graph genes_graph(bilab::gGraph{CLIARG::o_graph_name});
  
  bilab::NameVertexMap name2vertex;
  bilab::NameVertexMap::iterator pos_u;
  bilab::NameVertexMap::iterator pos_v;
  bool inserted;
  bilab::Vertex u, v;

  for(int i = 0; i < sel_inx.size() - 1; i++) {
    int n1_inx = sel_inx[i];
    std::string n1 = rownames[n1_inx];
    double d_n1 = (*dat)(n1_inx, 0);
    boost::tie(pos_u, inserted) = name2vertex.insert(
                                      std::make_pair(n1, bilab::Vertex()));
    if (inserted) {
      u = boost::add_vertex(bilab::gVertex{n1, d_n1}, genes_graph);
      pos_u->second = u;
    } else {
      u = pos_u->second;
    }

    for (int j = i + 1; j < sel_inx.size(); j++) {
      int n2_inx = sel_inx[j];
      std::string n2 = rownames[n2_inx];
      double d_n2 = (*dat)(n2_inx, 0);
      // Create two vertices in the graph
      boost::tie(pos_v, inserted) = name2vertex.insert(
                                    std::make_pair(n2, bilab::Vertex()));
      if (inserted) {
        v = boost::add_vertex(bilab::gVertex{n2, d_n2}, genes_graph);
        pos_v->second = v;
      } else {
        v = pos_v->second;
      }
      double d = abs(d_n1 - d_n2);
      if (d < CLIARG::d_threshold ) {
        // Create an edge conecting those two vertices
        boost::add_edge(u, v, bilab::gEdge{d}, genes_graph);
      }
    }
  }
  if (CLIARG::verbose) {
    timer.stop();
    seconds = std::chrono::nanoseconds(timer.elapsed().user);
    std::cout << " Completed in " << seconds.count() << " seconds." << std::endl;
  } else {
    std::cout << " Completed." << std::endl;
  }
  if (CLIARG::o_graph) {
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
