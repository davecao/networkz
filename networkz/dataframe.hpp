//
//  dataframe.hpp
//  networkz
//
//  Created by CAO Wei on 2020/08/20.
//  Copyright © 2020 曹巍. All rights reserved.
//

#ifndef dataframe_hpp
#define dataframe_hpp

#include <stdio.h>
#include <string>
#include <map>
#include <vector>
#include <iostream>

#include <Eigen/Core>

namespace NARO {

typedef Eigen::MatrixXd Dynamic2D;

// Data structure for CSV format based on Eigen::MatrixXd
struct Index
{
  std::map<std::string, size_t> Names;
  std::vector<std::pair<std::string, size_t>> OrderedNames;
  bool sorted = false;

  ssize_t GetIndex(std::string& name) const;
  std::vector<std::string> GetIndexNames();

private:
  void SortIndex();
};

struct DataFrame
{
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
  size_t num_rows = 0;
  size_t num_cols = 0;

  Index *rowIndex;
  Index *columnIndex;
  
  Dynamic2D data;

  DataFrame();
  DataFrame(size_t nrows, size_t ncols);
  ~DataFrame();

  void resize(size_t nrows, size_t ncols);
  size_t size();
  void set_data(Dynamic2D& d);
  
  // Overload operators
  double operator()(int r_inx, int c_inx);
   
  // Operations for indices
  // For columns
  bool set_columnIndex_names(std::string& col_name, size_t i);
  bool set_columnIndex_names(std::vector<std::string>& col_name);
  std::vector<std::string> get_columnIndex_names();
  // For rows
  bool set_rowIndex_names(std::vector<std::string>& row_name);
  std::vector<std::string> get_rowIndex_names();

  bool add_row(std::vector<std::string>& row, size_t rInx);
  //
  // Select data by a column name and return a copy of sub-dataset.
  // time-consuming and may cause the memory leaks
  //
  DataFrame select(std::string& c_name);
  //
  // Modify the passed dataframe and store the sub dataset.
  // Release the memory from the main function to avoid the memory leaks.
  //
  bool select(std::string& c_name, DataFrame* sliced);
};
} // Namespace NARO
#endif /* dataframe_hpp */
