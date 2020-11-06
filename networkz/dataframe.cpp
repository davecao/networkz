//
//  dataframe.cpp
//  networkz
//
//  Created by CAO Wei on 2020/08/20.
//  Copyright © 2020 曹巍. All rights reserved.
//
#include "utility.hpp"
#include "dataframe.hpp"

// -----------------------------------------------------------------------------
// bilab::Index::GetIndex() const
//
size_t NARO::Index::GetIndex(std::string& name) const {
  auto it = Names.find(name);
  if ( it != Names.end())
  {
    return it->second;
  }
  return -1;
}
// -----------------------------------------------------------------------------
// bilab::Index::GetIndexNames()
//
std::vector<std::string> NARO::Index::GetIndexNames() {
  std::vector<std::string> keys;
  if (!this->sorted) {
    this->SortIndex();
    this->sorted = true;
  }
  for (const auto& [key, ignored] : OrderedNames) {
      keys.push_back(key);
  }
  return keys;
}
// -----------------------------------------------------------------------------
// bilab::Index::SortIndex()
//
void NARO::Index::SortIndex() {
  // Construct a vector for sorting values
  std::vector<std::pair<std::string, size_t>> v{
    std::make_move_iterator(begin(this->Names)),
    std::make_move_iterator(end(this->Names))};
  // Sorting
  /*
  std::sort(std::execution::par, begin(v), end(v),
            [](auto p1, auto p2){return p1.second < p2.second;});
   */
  std::sort(begin(v), end(v),
            [](auto p1, auto p2){return p1.second < p2.second;});
  this->OrderedNames = v;
}

// -----------------------------------------------------------------------------
// bilab::DataFrame()
//
NARO::DataFrame::DataFrame() {
  this->rowIndex = new Index();
  this->columnIndex = new Index();
}
// -----------------------------------------------------------------------------
// bilab::DataFrame()
//
NARO::DataFrame::DataFrame(size_t nrows, size_t ncols) {
  this->num_rows = nrows;
  this->num_cols = ncols;
  this->data.resize(nrows, ncols);
  this->rowIndex = new Index;
  this->columnIndex = new Index;
}
// -----------------------------------------------------------------------------
//
NARO::DataFrame::DataFrame(const DataFrame &other) {
  this->num_rows = other.num_rows;
  this->num_cols = other.num_cols;
  this->data.resize(this->num_rows, this->num_cols);
  this->rowIndex = other.rowIndex;
  this->columnIndex = other.columnIndex;
  // copy data
  this->data = other.data;
}
// -----------------------------------------------------------------------------
// bilab::DataFrame::~DataFrame()
//
NARO::DataFrame::~DataFrame() {
  delete rowIndex;
  delete columnIndex;
}
// -----------------------------------------------------------------------------
// bilab::DataFrame::resize()
//
void NARO::DataFrame::resize(size_t nrows, size_t ncols) {
  this->num_rows = nrows;
  this->num_cols = ncols;
  this->data.resize(nrows, ncols);
}
// -----------------------------------------------------------------------------
// bilab::DataFrame::size()
//
size_t NARO::DataFrame::size() {
  if (num_rows != this->data.rows()){
    return this->data.rows();
  }
  return num_rows;
}
// -----------------------------------------------------------------------------
// bilab::DataFrame::set_data()
//
void NARO::DataFrame::set_data(Dynamic2D& d) {
  this->data = d;
}
// -----------------------------------------------------------------------------
// bilab::DataFrame::operator()
//
double NARO::DataFrame::operator()(int r_inx, int c_inx) {
  return this->data(r_inx, c_inx);
}
// -----------------------------------------------------------------------------
// bilab::DataFrame::set_columnIndex_names()
//
bool NARO::DataFrame::set_columnIndex_names(std::string& col_name, size_t i){
  this->columnIndex->Names.insert(
      std::pair<std::string, size_t>(col_name, i));
  return true;
}
// -----------------------------------------------------------------------------
// bilab::DataFrame::set_columnIndex_names()
//
bool NARO::DataFrame::set_columnIndex_names(std::vector<std::string>& col_name){
  // Sort the column name
  
  auto sorted_colname = col_name;
  std::sort(sorted_colname.begin(), sorted_colname.end());
  auto duplicate = std::adjacent_find(sorted_colname.begin(),
                                      sorted_colname.end());
  if (duplicate != sorted_colname.end()) {
    std::cout << "Found duplicate column name = " << *duplicate << "\n";
    std::exit(-1);
  }
  for(size_t i = 0; i != col_name.size(); i++){
    this->columnIndex->Names.insert(
      std::pair<std::string, size_t>(col_name[i], i));
  }
  return true;
}
// -----------------------------------------------------------------------------
// bilab::DataFrame::get_columnIndex_names()
//
std::vector<std::string> NARO::DataFrame::get_columnIndex_names() {
  return this->columnIndex->GetIndexNames();
}
// -----------------------------------------------------------------------------
// bilab::DataFrame::set_rowIndex_names()
//
bool NARO::DataFrame::set_rowIndex_names(std::vector<std::string>& row_name) {
  for(size_t i = 0; i != row_name.size(); i++){
    this->rowIndex->Names.insert(
      std::pair<std::string, size_t>(row_name[i], i));
  }
  return true;
}
// -----------------------------------------------------------------------------
// bilab::DataFrame::get_rowIndex_names()
//
std::vector<std::string> NARO::DataFrame::get_rowIndex_names() {
  return this->rowIndex->GetIndexNames();
}
// -----------------------------------------------------------------------------
// bilab::DataFrame::add_row()
//
bool NARO::DataFrame::add_row(std::vector<std::string>& row, size_t rInx){
  if ((row.size() - 1) != num_cols) {
    std::cerr << "Malformat: the number of columns does not match"
    << std::endl;
      return false; //std::exit(-1);
  }
  // Row index
  this->rowIndex->Names.insert(std::pair<std::string, size_t>(row[0], rInx));
  // Data
  for(size_t i = 1; i <= num_cols; i++){
    //std::cout << row[0] <<" at Line " << rInx << "." <<std::endl;
    this->data(rInx, i-1) = std::stod(row[i]);
  }
  return true;
}

// -----------------------------------------------------------------------------
// bilab::DataFrame::select()
//
// Select data by a column name and return a copy of sub-dataset.
// time-consuming and may cause the memory leaks
//
NARO::DataFrame NARO::DataFrame::select(std::string& c_name) {

  size_t inx = this->columnIndex->GetIndex(c_name);
  std::vector<std::string> rinx = this->rowIndex->GetIndexNames();
  if (inx == -1) {
    // Column name - Not found
    std::cout<< c_name << " Not found." << std::endl;
    std::exit(-1);
  }
  Dynamic2D sub = this->data(Eigen::all, inx-1);
  DataFrame *sliced = new DataFrame(sub.rows(), sub.cols());
  sliced->set_columnIndex_names(c_name, 0);
  sliced->set_rowIndex_names(rinx);
  sliced->set_data(sub);

  return *sliced;
}

// -----------------------------------------------------------------------------
// bilab::DataFrame::select()
//
//
// Modify the passed dataframe and store the sub dataset.
// Release the memory from the main function to avoid the memory leaks.
//
bool NARO::DataFrame::select(std::string& c_name, DataFrame* sliced)
{
  std::vector<std::string> rinx = this->rowIndex->GetIndexNames();
  size_t inx = this->columnIndex->GetIndex(c_name);
  if (inx == -1) {
    // Column name - Not found
    //std::cout<< c_name << " Not found." << std::endl;
    return false;
  }
  Dynamic2D sub = this->data(Eigen::all, inx-1);
  sliced->resize(sub.rows(), sub.cols());
  //DataFrame *sliced = new DataFrame(sub.rows(), sub.cols());
  sliced->set_columnIndex_names(c_name, 0);
  sliced->set_rowIndex_names(rinx);
  sliced->set_data(sub);

  return true;
}
// -----------------------------------------------------------------------------
// bilab::DataFrame::select()
//
bool NARO::DataFrame::select(std::vector<std::string>& c_names,
                             DataFrame* sliced)
{
  // Check the column names in dataframe
  int inx;
  std::vector<int> c_inx;
  for (auto c_name : c_names) {
    inx = static_cast<int>(this->columnIndex->GetIndex(c_name));
    if (inx == -1) {
      // Column name - Not found
      std::cerr<< "Specified column name, " << c_name <<
                  ", is not found in the data file." << std::endl;
      return false;
    }
    c_inx.push_back(inx-1);
  }
  
  std::vector<std::string> rinx = this->rowIndex->GetIndexNames();
  
  Dynamic2D sub = this->data(Eigen::all, c_inx);
  sliced->resize(sub.rows(), sub.cols());
  // Create new indices
  sliced->set_columnIndex_names(c_names);
  sliced->set_rowIndex_names(rinx);
  sliced->set_data(sub);

  return true;
}
// -----------------------------------------------------------------------------
// head()
//
void NARO::DataFrame::head(int n)
{
  int nrows = static_cast<int>(this->num_rows);

  std::vector<std::string> col_names = this->get_columnIndex_names();
  std::vector<std::string> row_names = this->get_rowIndex_names();
  if (this->num_rows != row_names.size()) {
    std::cerr << "The number of rows is not consistent with that of row labels"
    << std::endl;
    std::exit(-1);
  }
  // print header
  std::cout<< "\t" << join(col_names, "\t") << std::endl;
  int n_headers = (n > nrows) ? n : nrows;
  for (int i=0; i<n_headers; i++) {
    std::cout << row_names[i] <<"\t" << this->data.row(i) << std::endl;
  }
}
// -----------------------------------------------------------------------------
// describe()
//
void NARO::DataFrame::describe()
{
  size_t nrows = this->num_rows;
  size_t ncols = this->num_cols;
  std::cout << "The shape of dataframe:\n";
  std::cout << "(" << nrows << "," << ncols << ")" <<std::endl;
  head();
  std::cout << "Column-wise Mean:" << std::endl;
  std::cout << this->data.colwise().mean() << std::endl;
}
