//
//  TSVReader.hpp
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
 * TSVReader inherits FileReaderBase, and automatically registered to the FileReaderFactory.
 */
class TSVReader: public FileReaderBase
{
public:

  /// Dataframe
  DataFrame* df = nullptr;
  /// field separator for tsv format
  std::string sep = "\t";
  /// the starting character to indicate comment lines
  std::string comment = "#";
  /// the position of the header line after the comment lines
  int header = 0;
  
  ///  Constructor
  TSVReader() = default;
  /// Deconstructor
  ~TSVReader() = default;

  /// Convert the class type to a string
  auto getType() const -> std::string override;
  /**
   * @brief print the class type
   */
  void show();
 
  /**
   * @brief the template function of reader
   *
   * @param[in] filename the path of the input file
   * @param[in] t argument defined by type T
   * @tparam T the template  argument of function argument
   */
  template<typename T>
  void read(const std::string& filename, T& t)
  {
    /// print out the variable
    std::cout << t << std::endl;
  }
  
  /**
   * @brief recursive varadic function
   */
  template<typename First, typename... Ts>
  void read(const std::string& filename, First& first, Ts&... args) {
    std::cout << first << ", ";
    read(filename, args...);
  }
  /**
   * @brief template specialization for read function
   *
   * @param[in] filename the path of the input file
   * @param[out] df the DataFrame variable to store the Eigen matrix
   * @param[in] sep the field separator
   * @param[in] comment the starting character to indicate the comment line
   * @param[in] header the starting number of the header line
   *
   */
  bool read(const std::string& filename, NARO::DataFrame* df,
            std::string& sep, std::string& comment, int header) override;
  
  /// Configure options (Not used currently)
  void config(NARO::DataFrame*, std::string&, std::string&, int);
};
} // End of namespace NARO
#endif /* TSV_READER_HPP */
