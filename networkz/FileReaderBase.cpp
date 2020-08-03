//
//  FileReaderBase.cpp
//  networkz
//
//  Created by 曹巍 on 2020/08/01.
//  Copyright © 2020 曹巍. All rights reserved.
//

#include <iostream>
#include "FileReaderBase.hpp"


// -----------------------------------------------------------------------------
//
auto NARO::FileReaderBase::getType() const -> std::string {
  return "FileReaderBase";
}
// -----------------------------------------------------------------------------
//
void NARO::FileReaderBase::show() {
  std::cout << "Class type =" << this->getType() << std::endl;
}

// -----------------------------------------------------------------------------
// NARO::FileReaderBase::read(const std::string& filename)
//
bool NARO::FileReaderBase::read(const std::string &filename) {
  std::cerr << "Not implemented. The derived class should overwrite"
            <<std::endl;
  return false;
}

// -----------------------------------------------------------------------------
bool NARO::FileReaderBase::read(
                const std::string& filename, NARO::DataFrame* df,
                std::string& sep, std::string& comment, int header)
{
  std::cerr << "Not implemented. The derived class should overwrite"
            <<std::endl;
  return false;
}
// -----------------------------------------------------------------------------
//
/*
template<typename... Ts>
bool NARO::FileReaderBase::read(const std::string& filename, Ts...) {
  std::cerr << "Not implemented. The derived class should overwrite"
            <<std::endl;
  return false;
}
*/
// -----------------------------------------------------------------------------
//
/*
template<typename... Ts>
void NARO::FileReaderBase::read(const std::string& filename, Ts... args) {
  this->read(filename, args...);
}
*/
// -----------------------------------------------------------------------------
//
/*
template<typename ...Args>
void NARO::FileReaderBase::config(Args&&... args) {
  std::cerr << "Not implemented. The derived class should overwrite"
            << std::endl;
}
*/
