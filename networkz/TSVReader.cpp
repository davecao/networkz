//
//  TSVReader.cpp
//  networkz
//
//  Created by CAO Wei on 2020/07/30.
//  Copyright © 2020 曹巍. All rights reserved.
//

#include "TSVReader.hpp"

// -----------------------------------------------------------------------------
// Register
//
namespace {
  NARO::ConcreteFactory<NARO::TSVReader> factoryBase("TSVReader");
}
// -----------------------------------------------------------------------------
//
auto NARO::TSVReader::getType() const -> std::string {
  return "TSVReader";
}

// -----------------------------------------------------------------------------
//
void NARO::TSVReader::show() {
  std::cout << "Class Type = " << this->getType() << std::endl;
}

// -----------------------------------------------------------------------------
//
//
void NARO::TSVReader::config(NARO::DataFrame* d, std::string& sep,
                             std::string& comment, int header) {
  this->df = d;
  this->sep = sep;
  this->comment = comment;
  this->header = header;
}

// -----------------------------------------------------------------------------
// Overwrite the parent class function `read`
//
bool NARO::TSVReader::read(const std::string& filename,
              NARO::DataFrame* df, std::string& sep, std::string& comment,
                           int header) {
  std::vector<std::string> lines;
  // read the file and skip comment lines
  lines = readFileToLines(filename, comment);

  std::vector<std::string> l;
  boost::split(l, lines[0], boost::is_any_of(sep));
  
  // Get the number of rows and columns
  size_t nrows = static_cast<size_t>(lines.size());
  size_t ncols = static_cast<size_t>(l.size());
  
  // Contain a header line or not?
  if (header < 0) {
    // No header line if header < 0
    df->resize(nrows, ncols);
  } else if (header == lines.size()){
    std::cerr << "No data found" << std::endl;
    std::exit(-1);
  } else if (header > lines.size()) {
    std::cerr << "Wrong input argument: header="<< header << std::endl;
    std::cerr << "The argument value is larger than available lines."
              << std::endl;
    std::exit(-1);
  } else {
    df->resize(nrows - 1, ncols - 1);
    // Set the header line
    boost::split(l, lines[header], boost::is_any_of(sep));
    if (!df->set_columnIndex_names(l)){
      std::cerr << "Failed to set the header line." << std::endl;
    }
  }

  // Read data
  size_t row_counter = 0;
  for(size_t i = header + 1; i < lines.size(); i++){
    // split the line by the separator, boost::split
    boost::split(l, lines[i], boost::is_any_of(sep));
    // Check the number of columns whether is consistent with the header cols
    if (l.size() != ncols) {
      std::cout << "The " << i << " line: #column != #header" << std::endl;
      std::exit(-1);
    }
    // Store record lines
    if (!df->add_row(l, row_counter)) {
      std::cerr << "Failed to parse the format at line " << i << std::endl;
    }
    // increase the index
    ++row_counter;
  }
  // Check row number whether it is equal to the number of stored row labels
  if (row_counter != df->get_rowIndex_names().size()) {
    std::cerr << "Error: duplicate rows exists.(" << row_counter <<
    " lines read, but "<< df->get_rowIndex_names().size() <<
    " unqiue row labels available" << std::endl;
    std::exit(-1);
  }
  return true;
}
