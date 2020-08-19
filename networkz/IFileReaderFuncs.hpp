//
//  IFileReaderFuncs.hpp
//  networkz
//
//  Created by 曹巍 on 2020/08/03.
//  Copyright © 2020 曹巍. All rights reserved.
//

#ifndef IFileReaderFuncs_h
#define IFileReaderFuncs_h

#include <string>
#include "dataframe.hpp"
#include "graph_auxiliary.hpp"

namespace NARO {

struct IFileReader
{
  
  IFileReader() = default;
  ~IFileReader() = default;
  
  // Definition of interface functions
  // Override in the subclass
  virtual bool read(const std::string& filename);
  virtual bool read(const std::string& filename, NARO::DataFrame* df,
                    std::string& sep, std::string& comment, int header);
};

}
#endif /* IFileReaderFuncs_h */
