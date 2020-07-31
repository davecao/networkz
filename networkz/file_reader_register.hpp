//
//  file_reader_register.hpp
//  networkz
//
//  Created by CAO Wei on 2020/07/30.
//  Copyright © 2020 曹巍. All rights reserved.
//

#ifndef file_reader_register_hpp
#define file_reader_register_hpp

#include "file_reader_factory.hpp"

namespace bilab {

template<typename T>
class FileReaderRegister {
public:
  FileReaderRegister(const char* id) {
    FileReaderFactory::get().registerGenerator (
       id,
       [](){ return static_cast<bilab::FileReaderBase*>(new T());}
    );
  }
};

} // End of FileReaderRegister
#endif /* file_reader_register_hpp */
