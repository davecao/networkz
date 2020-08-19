//
//  file_reader.hpp
//  networkz
//
//  Created by CAO Wei on 2020/07/30.
//  Copyright © 2020 曹巍. All rights reserved.
//

#ifndef FILE_READER_BASE_H
#define FILE_READER_BASE_H

#include <string>
#include "dataframe.hpp"
#include "IFileReaderFuncs.hpp"

namespace NARO {

class FileReaderBase : public IFileReader
{
  
public:

  FileReaderBase() = default;
  //Destructor of base class must always be virtual
  virtual ~FileReaderBase() = default;
  
  virtual auto getType() const -> std::string;
  void show();
  
  // Interface functions
  virtual bool read(const std::string& filename);
  virtual bool read(const std::string& filename, NARO::DataFrame* df,
                    std::string& sep, std::string& comment, int header);
};

}
#endif /* FILE_READER_BASE_H */
