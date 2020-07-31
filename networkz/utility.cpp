//
//  utility.cpp
//  networkz
//
//  Created by CAO Wei on 2020/07/30.
//  Copyright © 2020 曹巍. All rights reserved.
//

#include <iostream>
#include <fstream>
#include "utility.hpp"

/**
 * Show Eigen version information.
 *
 */
void show_eigen_info() {
  std::cout << "Eigen version: "
            << EIGEN_WORLD_VERSION << "."
            << EIGEN_MAJOR_VERSION << "."
            << EIGEN_MINOR_VERSION
  << std::endl;
}

/**
 *  Boost version information.
 *
 */
void show_boost_version() {
  std::cout << "Boost version: "
            << BOOST_VERSION / 100000     << "."  // major version
            << BOOST_VERSION / 100 % 1000 << "."  // minor version
            << BOOST_VERSION % 100                // patch level
  << std::endl;
}

void welcome_message() {
  std::cout << "Networkz is using the following packages:" <<std::endl;
  show_boost_version();
  show_eigen_info();
}

std::string byteConverter(long long byte)
{
  std::stringstream ss;
  if (byte < 1024){
    ss << byte <<" Bytes";
  }else if (byte <1048576 && byte >= 1024){
    ss << static_cast<float>(byte/1024) <<" KB";
  }else if (byte < 1073741824 && byte >= 1048576) {
    ss << static_cast<float>(byte/1048576) <<" MB";
  }else{
    ss << static_cast<float>(byte/1073741824)<<" GB";
  }
  return ss.str();
}

std::string byteConverter_s(long long byte)
{
  std::stringstream ss;
  if (byte < 1000){
    ss << byte <<" Bytes";
  }else if (byte <1000000 && byte >= 1000){
    ss << static_cast<float>(byte)/1000 <<" KB";
  }else if (byte < 1000000000 && byte >= 1000000) {
    ss << static_cast<float>(byte)/1000000 <<" MB";
  }else{
    ss << static_cast<float>(byte)/1000000000<<" GB";
  }
  return ss.str();
}

std::string get_local_time() {
  std::time_t t = std::time(nullptr);
  std::tm* now = std::localtime(&t);
  std::stringstream ss;
  ss << (now->tm_year + 1900) << "-"
     << (now->tm_mon + 1) << "-"
  << now->tm_mday;
  return ss.str();
}

std::vector<std::string> readFileToLines(const std::string& file,
                                         const std::string& comment="#")
{
  namespace fs = std::filesystem;
  std::vector<std::string> lines;
  // 1. Check the existance of the tsv file
  if (!fs::exists(file) && fs::is_regular_file(file))
  {
    std::cout << "Could not find the input file: "<< file << std::endl;
  }
  // 2. Open the file by Stream
  std::ifstream f(file);
  if (f) {
    std::string str;
    // Read the next line from File untill it reaches the end.
    while (std::getline(f, str))
    {
      // Remove the newline
      str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
      // Remove the comment lines
      if (str.substr(0, comment.length()) == comment){
        continue;
      }
      // Line contains string of length > 0 then save it in vector
      if(str.size() > 0)
          lines.push_back(str);
    }
  } else {
    std::cout << "Failed to open the file, "<< file << "." <<std::endl;
  }
  // 3. close
  f.close();
  return lines;
}
