//
//  util.hpp
//  networkz
//
//  Created by 曹巍 on 2020/07/25.
//  Copyright © 2020 曹巍. All rights reserved.
//

#ifndef util_h
#define util_h

// all the system #include's we'll ever need
#include <string>
#include <iostream>
#include <filesystem>
//Boost
#include <boost/config.hpp> // put this first to suppress some VC++ warnings
#include <boost/program_options.hpp>
#include <boost/thread.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/timer/timer.hpp>
#include <boost/format.hpp>
#include <boost/bind.hpp>
#include <boost/pending/stringtok.hpp>
#include <boost/regex.hpp>
#include <boost/any.hpp>
#include <boost/iostreams/device/mapped_file.hpp>

std::uintmax_t ComputeFileSize(const fs::path& pathToCheck)
{
  if (fs::exists(pathToCheck) && fs::is_regular_file(pathToCheck))
  {
    auto err = std::error_code{};
    auto filesize = fs::file_size(pathToCheck, err);
    if (filesize != static_cast<uintmax_t>(-1))
      return filesize;
  }

  return static_cast<uintmax_t>(-1);
}

std::string readFileToString(const std::string& file)
{
  // 1. Check the existance of the tsv file
  if (!fs::exists(file) && fs::is_regular_file(file))
  {
    std::cout << "Could not find the input file: "<< file << std::endl;
  }
  std::ifstream t(file);
  std::string str(
                  (std::istreambuf_iterator<char>(t)),
                   std::istreambuf_iterator<char>());
  t.close();
  return str;

}

std::vector<std::string> readFileByMMap(const std::string& file)
{
  std::vector<std::string> lines;
  // 1. Check the existance of the tsv file
  if (!fs::exists(file) && fs::is_regular_file(file))
  {
    std::cout << "Could not find the input file: "<< file << std::endl;
  }
  boost::iostreams::mapped_file mmap(file,
                                     boost::iostreams::mapped_file::readonly);
  using iterator = char const*;
  
  return lines;
}
#endif /* util_h */
