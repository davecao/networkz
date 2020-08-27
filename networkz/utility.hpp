//
//  utility.hpp
//  networkz
//
//  Created by CAO Wei on 2020/07/30.
//  Copyright © 2020 曹巍. All rights reserved.
//

#ifndef utility_h
#define utility_h

#include <Eigen/Core>

#include <iostream>
#include <vector>
#include <string>
//#include <filesystem>
#ifdef __cpp_lib_system
#include <filesystem>
namespace NARO {
  namespace fs = std::filesystem;
  typedef std::error_code ErrorCode;
}
#elif __cpp_lib_experimental_filesystem
#include <experimental/filesystem>
namespace NARO {
  namespace fs = std::experimental::filesystem;
  typedef fs::error_code ErrorCode;
}
#else
#include <boost/filesystem.hpp>
namespace NARO {
  namespace fs = boost::filesystem;
  typedef boost::system::error_code ErrorCode;
}
#endif

#include <boost/version.hpp>

void welcome_message();
std::string byteConverter(long long byte);
std::string byteConverter_s(long long byte);
std::string get_local_time();
std::string repeat(std::string str, const std::size_t n);
std::vector<std::string> readFileToLines(const std::string& file,
                                         const std::string& comment);

#endif /* utility_h */
