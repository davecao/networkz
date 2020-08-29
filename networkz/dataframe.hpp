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

// Data structure for CSV format based on Eigen::MatrixXd

///< Define a type of 2D matrix
typedef Eigen::MatrixXd Dynamic2D;

/**
 * @brief String labels for indexing
 *
 * A row or column string labels of a matrix
 */
struct Index
{
  /// A map of string to int
  std::map<std::string, size_t> Names;
  /// A sorted sequence by string labels
  std::vector<std::pair<std::string, size_t>> OrderedNames;
  /// The string labels had been sorted or not
  bool sorted = false;
  /**
   *  Convert the string labels to the numerical index
   *
   * @param[in] name get the numerical index for a string label of the column name
   */
  size_t GetIndex(std::string& name) const;
  /**
   * @brief obtain all string labels
   *
   * @returns a vector of string
   */
  std::vector<std::string> GetIndexNames();

private:
  /**
   * @brief Sort the map of string labels - number indices
   */
  void SortIndex();
};

/**
 * @brief A wrapper of Eigen matrix
 *
 * DataFrame is a wrapper class for the Eigen matrix to provide string indicing
 */
struct DataFrame
{
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
  /// Record the number of rows of the dataframe stored
  size_t num_rows = 0;
  /// Record the number of cols of the dataframe stored
  size_t num_cols = 0;
  /// The row string labels for indexing
  Index *rowIndex;
  /// The column string labels for indexing
  Index *columnIndex;
  /// The wrapped Eigen matrix
  Dynamic2D data;
  
  /**
   * Constructor
   *
   *  Initialize the dataframe without arguments
   */
  DataFrame();
  
  /**
   * Constructor
   *
   * Initialize the dataframe with the specified rows and columns
   *
   * @param[in] nrows the number of rows
   * @param[in] ncols the number of columns
   */
  DataFrame(size_t nrows, size_t ncols);
  
  /**
   * Copy constructor
   */
  DataFrame(const DataFrame &other);
  /**
   * Deconstructor
   */
  ~DataFrame();

  /**
   * Resize the dataframe to a new size
   *
   * @param[in] nrows resize to be `nrows` rows
   * @param[in] ncols resize to be `ncols` columns
   */
  void resize(size_t nrows, size_t ncols);
  
  /**
   * Obtain the number of rows of the dataframe
   */
  size_t size();
  /**
   * Set the matrix data
   *
   * @param[in] d A `double` type Eigen matrix
   */
  void set_data(Dynamic2D& d);

  /**
   * Overloaded operator() for easily accessing a matrix element by a pair of row and column indice
   *
   * @param[in] r_inx the row index (An integer)
   * @param[in] c_inx the column index (An integer)
   * @returns A double matrix element
   */
  double operator()(int r_inx, int c_inx);
   
  // For columns
  /**
   * @brief Operation for columns
   *
   * Assign the string label to its corresponding numerical index
   *
   * @param[in] col_name the string label
   * @param[in] i the corresponding numerical index
   * @returns bool, true for successfully inserting the pair of string-number index
   */
  bool set_columnIndex_names(std::string& col_name, size_t i);
  
  /**
   * @brief Operation for columns
   *
   * Assign a vector of string labels
   *
   * @param[in] col_name A vector of string labels
   * @returns bool, true for success
   */
  bool set_columnIndex_names(std::vector<std::string>& col_name);
  /**
   * @brief Obtain a vector of  string labels for columns
   *
   * @returns std::vector of string, string labels of columns
   */
  std::vector<std::string> get_columnIndex_names();
  // For rows
  /**
   * @brief Operation for rows
   *
   * Assign the row indices to the dataframe
   *
   * @param[in] row_name A vector of string labels of rows
   * @returns bool, true for success
   */
  bool set_rowIndex_names(std::vector<std::string>& row_name);
  /**
   * @brief Obtain a vector of string labels of  rows
   *
   * @returns std::vector of string, string labels of rows
   */
  std::vector<std::string> get_rowIndex_names();

  /**
   * @brief Add a row data to the dataframe by specifying its row index
   *
   * @param[in] row the row data as a vector of string
   * @param[in] rInx the numerical index of the row
   * @returns bool, true for success; false for failure
   */
  bool add_row(std::vector<std::string>& row, size_t rInx);
  /**
   * @brief Obtain a column data by a string label of the column
   *
   * Select data by a column name and return a copy of sub-dataset.
   * @param[in] c_name the string label of the column
   * @note it is time-consuming and may cause the memory leaks
   */
  DataFrame select(std::string& c_name);
  /**
   * @brief A subroutine for column selection by a string label
   *
   * Modify the passed dataframe and store the sub dataset.
   * Release the memory from outside of the function to avoid the memory leaks.
   *
   * @param[in] c_name the string label of the column
   * @param[out] sliced store the column data
   * @returns bool, true for success; false for failure
   */
  bool select(std::string& c_name, DataFrame* sliced);
  /**
   * @brief A subroutine for column selection by a group of string labels
   *
   *  Obtain the group of columns by string labels
   *
   *  @param[in] c_names a vector of string labels of columns
   *  @param[out] sliced store the column data
   *  @returns bool, true for success; false for failure
   */
  bool select(std::vector<std::string>& c_names, DataFrame* sliced);
  /**
   * @brief print the detailed info of the dataframe
   *
   */
  void describe();
  //
  /**
   * @brief print the first `n` rows in the dataframe
   *
   * @param[in] n the number of rows to be displayed.
   */
  void head(int n);
};
} // Namespace NARO
#endif /* dataframe_hpp */
