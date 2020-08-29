//
//  FileReaderBase.hpp
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
/**
 * @brief Base class for file readers
 *
 * The newly developed file reader must be derived from this base class.
 */
class FileReaderBase : public IFileReader
{
  
public:
  /// Constructor
  FileReaderBase() = default;
  
  /// Destructor of base class must always be virtual
  virtual ~FileReaderBase() = default;
  /// Convert the class type to be a string
  virtual auto getType() const -> std::string;
  /// Print the class type info
  void show();
  
  /// Functions defined in the IFileReader must be implemented in the derived class.
  virtual bool read(const std::string& filename);
  /// Functions defined in the IFileReader must be implemented in the derived class.
  virtual bool read(const std::string& filename, NARO::DataFrame* df,
                    std::string& sep, std::string& comment, int header);
};

}
#endif /* FILE_READER_BASE_H */
