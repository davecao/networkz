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

#include "common.hpp"

namespace po = boost::program_options;

namespace VersionInfo{
  std::string version("Networkz Version 1.0 developped by CAO Wei.");
}

namespace CLIARG {
  // General options
  bool verbose = false; // output detailed info if true.
  bool o_graph = false; // output graphviz file if true.
  
  std::string i_filename; // input file name
  std::string o_filename = "o_graphviz.gv"; // default file name for graphviz.
  std::string inputfile_size; // store the size of input file.
  std::string column_name = "tpm"; // the name of the column in the input file.
  // Graph options
  std::string o_graph_name = "Gene Expression Network"; // title for graphviz
  double d_threshold = 0.0; // the threshold for distance cutoff.
  
  // all the options
  po::options_description general("General options");
  po::options_description opt("Graph options");
  
  // Function: Initialize opts objects
  void init(){
    general.add_options()
    ("version,V", "Show the version number")
    ("infile,i",po::value<std::string>(&i_filename),"The input data file. Only the tsv format is supported now.")
    ("outfile,o", po::value<std::string>(&o_filename),"The output file of clusters in text format. Default is 'o_graphviz.gv'.")
    ("column,c", po::value<std::string>(&column_name), "The column name in the input file. Default is 'tpm'")
    ("graph,g","Output the graph to a file in the graphviz format if true.")
    ("help,h", "print help info.")
    ;
    
    opt.add_options()
    ("verbose,v","The extra verbose.")
    ("threshold,t",po::value<double>(&d_threshold),"The threshold between two vertices which are linked by an edge if the distance less than it. default is 0.")
    ("title,n",po::value<std::string>(&o_graph_name),"The title of the graph used to label the png file. Default is Gene Expression Network.")
    ;
    general.add(opt);
  }
  
  // Function: Check the existance of the file
  //
  bool FileExists(std::string fname) {
    struct stat FileInfo;
    int val;
    val = stat(fname.c_str(), &FileInfo);
    //if(FileInfo.st_size >1000000){
      //inputfile_size = byteConverter_s(FileInfo.st_size);
      //std::cout<<fname<<" is so large ("<<inputfile_size<<")"<<". Take a while to analysis..."<<std::endl;
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
      exit(0);
    }
    
    if( vm.count("version") ){
      //print version info
      std::cout << VersionInfo::version << std::endl;
      exit(0);
    }
    if ( vm.count("verbose") ){
      verbose = true;
    }
    if ( vm.count("title") ) {
      o_graph_name = vm["title"].as<std::string>();
    }
    if ( vm.count("outfile") ) {
      o_filename = vm["outfile"].as<std::string>();
    }
    if ( vm.count("column") ) {
      column_name = vm["column"].as<std::string>();
    }
    if ( vm.count("graph") ) {
      o_graph = true;
    }
    if ( vm.count("threshold") ) {
      d_threshold = vm["threshold"].as<double>();
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
      exit(0);
    }
  }
}
#endif /* cli_opt_h */
