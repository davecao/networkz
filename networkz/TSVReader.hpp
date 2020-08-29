//
//  reader_tsv.hpp
//  networkz
//
//  Created by CAO Wei on 2020/07/30.
//  Copyright © 2020 曹巍. All rights reserved.
//

#ifndef TSV_READER_HPP
#define TSV_READER_HPP

#include "common.hpp"
#include "graph.hpp"
#include "utility.hpp"
#include "FileReaderFactory.hpp"
#include "FileReaderBase.hpp"

namespace NARO {
/**
 * @brief A reader for the TSV file
 *
 * TSVReader inherits \link FileReaderBase\endlink, and automatically registered to the
 * \link FileReaderFactory\endlink.
 */
class TSVReader: public FileReaderBase
{
public:

  DataFrame* df = nullptr;
  std::string sep = "\t";
  std::string comment = "#";
  int header = 0;
  
  TSVReader() = default;
  ~TSVReader() = default;

  auto getType() const -> std::string;
  void show();

  // overwrite base function
  //void read(const std::string& filename, bool success);
  
  //
  template<typename T>
  void read(const std::string& filename, T& t)
  {std::cout << t << std::endl;}
  
  // recursive varadic function
  template<typename First, typename... Ts>
  void read(const std::string& filename, First& first, Ts&... args) {
    std::cout << first << ", ";
    read(filename, args...);
  }
  // specialization
  bool read(const std::string& filename, NARO::DataFrame* df,
            std::string& sep, std::string& comment, int header);
  
  // Configure options
  void config(NARO::DataFrame*, std::string&, std::string&, int);
};
} // End of namespace NARO
#endif /* TSV_READER_HPP */
