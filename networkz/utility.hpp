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
#include <filesystem>

#include <boost/version.hpp>

void welcome_message();
std::string byteConverter(long long byte);
std::string byteConverter_s(long long byte);
std::string get_local_time();
std::vector<std::string> readFileToLines(const std::string& file,
                                         const std::string& comment);

#endif /* utility_h */
