//
//  cli_opt.h
//  networkz
//
//  Created by 曹巍 on 2020/07/25.
//  Copyright © 2020 曹巍. All rights reserved.
//

#ifndef cli_opt_h
#define cli_opt_h

#include <sys/stat.h>
#include <array>

#include "common.hpp"

namespace po = boost::program_options;

namespace VersionInfo{
  std::string version("Networkz Version 1.0 developped by CAO Wei.");
}

namespace CLIARG {
  // General options
  bool verbose = false; ///< output detailed info if true.
  
  std::string config_file = "networkz.cfg"; ///< configuration file
  std::string parser_name = "TSVReader"; ///< parser name "TSVReader"
  std::string i_filename; ///< input file name
  std::string o_filename = "describe_networkz.log"; ///< default file name for graphviz.
  std::string inputfile_size; ///< store the size of input file.
  std::vector<std::string> column_names; ///< the name of columns selected in the input file.

  // Graph options
  std::string o_graph_name = "Gene Expression Network"; ///< title for graphviz
  std::string o_graph_file = "";  ///< output graphviz file name
  // Distance type for graph construction, default is "city".
  std::string mst_algo_name = "prim"; ///< Name of minimum spanning tree
  std::string distance_type = "city"; ///< distance method
  /**
   * Support distance type:
   *  "city": city-block distance
   *  "euc" : euclidean distance
   *  "corr": pearson's correlation coefficient
   */
  std::array<std::string, 3> d_type_supported{"city", "euc", "corr"};
  std::array<std::string, 2> mst_type_supported{"kruskal", "prim"};
  double d_threshold = 0.01; ///< the threshold for distance cutoff.
  
  // all the options
  po::options_description general("General options");
  po::options_description opt("Graph options");
  /**
   *  Command line options
   *
   *  Initialization function
   */
  void init(){
    general.add_options()
    ("version,V", "Show the version number")
    ("config", po::value<std::string>(&config_file),
             "Specify the path of a configuration file to analyze the graph.(NOT FINISHED)")
    ("parser,p", po::value<std::string>(&parser_name),
              "File parser name. Default is 'TSVReader'.")
    ("infile,i", po::value<std::string>(&i_filename)->required(),
               "The input data file. Only the tsv format is supported now.")
    ("outfile,o", po::value<std::string>(&o_filename),
              "The output file of clusters in text format."
              "Default is 'describe_networkz.log'.")
    ("column,c", po::value<std::vector<std::string>>(&column_names)->multitoken(),
                "Specify the column names in the input file.")
    ("graph,g", po::value<std::string>(&o_graph_file),
               "Output the graph to a file in the graphviz format.")
    ("help,h", "print help info.")
    ;
    
    opt.add_options()
    ("verbose,v", "The extra verbose.")
    ("threshold,t", po::value<double>(&d_threshold),
              "The threshold between two vertices which are linked by an edge "
              "if the distance less than it. "
              "default is 0.01.")
    ("distance,d", po::value<std::string>(&distance_type),
              "The distance type(string): \n "
              "'city' for city-block distance.\n "
              "'euc' for euclidean distance.\n "
              "'corr' for pearson correlation coefficient.")
    ("mst,m", po::value<std::string>(&mst_algo_name),
              "Algorithm names of Minmum spanning tree(undirected graph):\n"
              "'kruskal' for Kruskal's algorithm."
              "'prim' for Prim's algorithm, choiced for the situation that"
              "       #edges are more than #vertices."
              "Default is 'prim'")
    ("title,n", po::value<std::string>(&o_graph_name),
              "The title of the graph used to label the png file. "
              "Default is 'Gene Expression Network'.")
    ;
    general.add(opt);
  }
  /**
   * @brief Check the existance of the file
   *
   * @param[in] fname the path of a file
   */
  bool FileExists(std::string fname) {
    struct stat FileInfo;
    int val;
    val = stat(fname.c_str(), &FileInfo);
    //if(FileInfo.st_size >1000000){
      //inputfile_size = byteConverter_s(FileInfo.st_size);
      //std::cout<<fname<<" is so large ("<<inputfile_size<<")"
      //         <<". Take a while to analysis..."<<std::endl;
    //}
    return (val == 0) ? true : false;
  }

  // Function: Parse cmdline arguments
  //
  void ParseCmdLine(int argc, const char * argv[]) {
    //
    //parse command lines
    po::variables_map vm;
    try{
      // initialize options
      init();
      po::store(parse_command_line(argc, argv, general),vm);
      po::notify(vm);
    }catch(po::ambiguous_option& e){
      //std::cout<<"Program is terminated for the following reason(s):\n\n";
      std::cout<<e.get_option_name()<<" is an ambiguous option name.\n";
      std::cout << general << std::endl;
      exit(0);
      //BOOST_CHECK_EQUAL(std::string(e.what()), "Unknown option");
    }catch(po::unknown_option& e){
      std::cout<<e.get_option_name()<<" is an unknown option name.\n";
      std::cout << general << std::endl;
      exit(0);
      //BOOST_CHECK_EQUAL(e.get_option_name(), "option name");
      //BOOST_CHECK_EQUAL(std::string(e.what()), "Unknown option");
    }
    
    if( vm.count("help") ){
      // print help info
      std::cout << general << std::endl;
      std::exit(0);
    }
    
    if( vm.count("version") ){
      //print version info
      std::cout << VersionInfo::version << std::endl;
      exit(0);
    }
    if ( vm.count("verbose") ){
      verbose = true;
    }
    if ( vm.count("distance") ) {
      // Check it was support or not?
      auto found = std::find(d_type_supported.begin(),
                             d_type_supported.end(),
                             distance_type);
      if (found == d_type_supported.end()) {
        std::cerr << "Error: '-d, distance' option value INCORRECT."
                  << std::endl;
        std::exit(-1);
      }
    }
    // Check required arguments
    if ( vm.count("infile") ) {
      i_filename = vm["infile"].as<std::string>();
      if( !FileExists(i_filename) ) {
        std::cout << "Could not find the file: "<< i_filename <<std::endl;
        std::cout << "Program terminated." << std::endl;
        exit(0);
      }
    }else{
      std::cout << general << std::endl;
      std::exit(0);
    }
  }
}
#endif /* cli_opt_h */
