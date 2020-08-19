//
//  common.h
//  networkz
//
//  Created by 曹巍 on 2020/07/25.
//  Copyright © 2020 曹巍. All rights reserved.
//

#ifndef common_h
#define common_h

// all the system #include's we'll ever need
#include <fstream>
#include <cmath>
#include <complex>
#include <iostream>
#include <iomanip>
#include <vector>
#include <list>
#include <map>
#include <stack>
#include <queue>
#include <algorithm>
#include <iterator>
#include <limits>   // numeric_limits
#include <typeinfo> // typeid
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <fcntl.h>
#include <string>
#include <string_view>  // since C++17,
#include <ctype.h>
#include <assert.h>
#include <filesystem>
//#include <execution> // parallel not supported by clang 9.0 yet
//#include <regex> // Not supported by clang++ with llvm 4.2.1

//Boost
#include <boost/config.hpp> // put this first to suppress some VC++ warnings
#include <boost/program_options.hpp>
#include <boost/thread.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/timer/timer.hpp>
#include <boost/format.hpp>
#include <boost/bind.hpp>
#include <boost/pending/stringtok.hpp>
#include <boost/regex.hpp>
#include <boost/any.hpp>
#include <boost/iostreams/device/mapped_file.hpp>

// Config
#include "config.hpp"
#endif /* common_h */
