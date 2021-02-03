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
#include <string>     // std::to_string
#include <algorithm>  // std::max_element

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
#  ifdef __has_include
#    if __has_include(<filesystem>)
#include <filesystem>
namespace NARO {
  namespace fs = std::filesystem;
  typedef std::error_code ErrorCode;
}
#    elif __has_include(<experimental/filesystem>)
#include <experimental/filesystem>
namespace NARO {
  namespace fs = std::experimental::filesystem;
  typedef fs::error_code ErrorCode;
}
#    endif
#  else
#include <boost/filesystem.hpp>
namespace NARO {
  namespace fs = boost::filesystem;
  typedef boost::system::error_code ErrorCode;
}
#  endif
#endif

#include <boost/version.hpp>
/**
 * @brief Show the boost and eigen version info
 */
void welcome_message();
/**
 * @brief Calculate the byte to human readable format string
 *
 * @param[in] byte The number of bytes
 * @returns a string for human readable
 */
std::string byteConverter(long long byte);

/**
 * @brief Calculate the byte to human readable format string
 *
 * @param[in] byte The number of bytes
 * @returns a string for human readable
*/
std::string byteConverter_s(long long byte);

/// Obtain the local time on the machine
std::string get_local_time();

/**
 * @brief join a vector of string with a delimiter
 *
 * @param[in] v A vector of string to be joined
 * @param[in] delimiter A string to be used as the separator
 * @returns A string
 */
 std::string join(const std::vector<std::string> & v,
                 const std::string & delimiter);
/**
 * @brief Repeat a given string `n` times
 *
 * @param[in] str A string to be repeated
 * @param[in] n The number of repeats
 * @returns A repeated string
 */
std::string repeat(std::string str, const std::size_t n);

/**
 * @brief Read a data file into a vector of string
 *
 * @param[in] file the path of the file
 * @param[in] comment the character indicates the comment line.
 * @returns A vector of string
 */
std::vector<std::string> readFileToLines(const std::string& file,
                                         const std::string& comment);

/**
 * @ brief Remove the line break safely
 *
 * @param[in] ifs a ifstream object
 * @param[out] str a stripped string
 * @returns an ifstream
 */
std::istream& safeGetLine(std::ifstream& is, std::string& str);

#endif /* utility_h */
