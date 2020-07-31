//
//  file_reader.cpp
//  networkz
//
//  Created by CAO Wei on 2020/07/30.
//  Copyright © 2020 曹巍. All rights reserved.
//
#include <stdio.h>
#include <stdlib.h>

#include "file_reader_factory.hpp"

// -----------------------------------------------------------------------------
bilab::FileReaderFactory::FileReaderFactory()
{}

bilab::FileReaderFactory::~FileReaderFactory()
{}

bilab::FileReaderBase*
bilab::FileReaderFactory::getFileReader(const char* typeName){
  auto it = m_generators.find(typeName);
  if(it != m_generators.end()){
    return it->second();
  }
  return nullptr;
}

bool bilab::FileReaderFactory::registerGenerator(
                            const char* typeName,
                            const readerInstanceGenerator& funcCreate) {
  return m_generators.insert(std::make_pair(typeName, funcCreate)).second;
}

const char**
bilab::FileReaderFactory::getMenu(int& count) {
  count = static_cast<int>(m_generators.size());
  const char** arrayHead = new const char*[count];
  int i = 0;
  for (auto g : m_generators) {
    size_t buffer_size = g.first.length() + 1;
    char* generatorIdBuffer = new char[buffer_size];
    //strncpy_s(generatorIdBuffer, buffer_size, g.first.c_str(), g.first.length());
    std::strncpy(generatorIdBuffer, g.first.c_str(), buffer_size);
    arrayHead[i++] = generatorIdBuffer;
  }
  return arrayHead;
}

bilab::FileReaderFactory& bilab::FileReaderFactory::get() {
  static FileReaderFactory instance;
  return instance;
}

bool read_tsv(const std::string& tsv_file,
              bilab::DataFrame* df,
              std::string sep="\t",
              std::string comment="#",
              int header=0) {
  
  std::vector<std::string> lines;
  // read the file and skip comment lines
  lines = readFileToLines(tsv_file, comment);

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

    // Store record lines
    if (!df->add_row(l, row_counter)) {
      std::cerr << "Failed to parse the format at line " << i << std::endl;
    }
    // increase the index
    ++row_counter;
  }
  return true;
}
