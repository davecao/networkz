//
//  file_reader.hpp
//  networkz
//
//  Created by CAO Wei on 2020/07/30.
//  Copyright © 2020 曹巍. All rights reserved.
//

#ifndef file_reader_h
#define file_reader_h

#include <string>
#include "file_reader_register.hpp"

namespace bilab {
class FileReaderBase {

public:
  virtual bool read(std::string& filename) const = 0;
};
}// End of namespace bilab

#endif /* file_reader_h */
