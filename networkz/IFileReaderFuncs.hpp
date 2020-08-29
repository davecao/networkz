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
#include "graph.hpp"

namespace NARO {
/**
 * @brief Interface for read functions implemented in the derived class
 */
struct IFileReader
{
  /// Constructor
  IFileReader() = default;
  /// Deconstructor
  ~IFileReader() = default;
  
  // Definition of interface functions
  // Override in the subclass
  /// Functions defined in the IFileReader must be implemented in the derived class.
  virtual bool read(const std::string& filename);
  /// Functions defined in the IFileReader must be implemented in the derived class.
  virtual bool read(const std::string& filename, NARO::DataFrame* df,
                    std::string& sep, std::string& comment, int header);
};

}
#endif /* IFileReaderFuncs_h */
