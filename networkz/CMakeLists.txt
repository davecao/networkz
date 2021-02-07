#
# File: CMakeLists.txt
# Project: networkz
# 
# 
# Copyright (c) 2021 国立研究開発法人 農業・食品産業技術総合研究機構 (NARO)
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy 
# of this software and associated documentation files (the 'Software'), to deal 
# in the Software without restriction, including without limitation the rights 
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
# copies of the Software, and to permit persons to whom the Software is furnished 
# to do so, subject to the following conditions:                                  
#                                                                                 
# The above copyright notice and this permission notice shall be included in all  
# copies or substantial portions of the Software.                                 
#                                                                                 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
# INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
# PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
# HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF 
# CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE 
# OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
# 
# Author: 曹巍, sou197@affrc.go.jp
# Created Date: 2021-01-16 09:11:05
# -----
# Last Modified: 2021-01-16 09:11:06
# Modified By: 
# -----
# HISTORY:
# Date      	By	Comments
# ----------	---	----------------------------------------------------------
#

cmake_minimum_required(VERSION 3.13)  # CMake version check
message(STATUS "Building using CMake version: ${CMAKE_VERSION}")

# Compiler id for Apple Clang is now AppleClang.
# https://www.cmake.org/cmake/help/latest/policy/CMP0025.html
cmake_policy(SET CMP0025 NEW)

# Only interpret if() arguments as variables or keywords when unquoted.
# https://www.cmake.org/cmake/help/latest/policy/CMP0054.html
cmake_policy(SET CMP0054 NEW)

# Support new if() IN_LIST operator.
# https://www.cmake.org/cmake/help/latest/policy/CMP0057.html
cmake_policy(SET CMP0057 NEW)

# Adapted from Apache Kudu: https://github.com/apache/kudu/commit/bd549e13743a51013585
# Honor visibility properties for all target types.
# https://www.cmake.org/cmake/help/latest/policy/CMP0063.html
cmake_policy(SET CMP0063 NEW)

# RPATH settings on macOS do not affect install_name.
# https://cmake.org/cmake/help/latest/policy/CMP0068.html
if(POLICY CMP0068)
  cmake_policy(SET CMP0068 NEW)
endif()

# find_package() uses <PackageName>_ROOT variables.
# https://cmake.org/cmake/help/latest/policy/CMP0074.html
if(POLICY CMP0074)
  cmake_policy(SET CMP0074 NEW)
endif()

# ----- [ Networkz version ] -----
# The project version number.
set(NETWORKZ_VERSION "1.0.0-SNAPSHOT")

string(REGEX MATCH "^[0-9]+\\.[0-9]+\\.[0-9]+" NETWORKZ_BASE_VERSION "${NETWORKZ_VERSION}")

# if no build build type is specified, default to release builds
if(NOT DEFINED CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build.")
endif()
string(TOLOWER ${CMAKE_BUILD_TYPE} LOWERCASE_BUILD_TYPE)
string(TOUPPER ${CMAKE_BUILD_TYPE} UPPERCASE_BUILD_TYPE)

project(networkz VERSION "${NETWORKZ_BASE_VERSION}" LANGUAGES C CXX )  # Create project "networkz"

set(CMAKE_CXX_STANDARD 17)      # Enable c++17 standard

set(NETWORKZ_VERSION_MAJOR "${networkz_VERSION_MAJOR}")
set(NETWORKZ_VERSION_MINOR "${networkz_VERSION_MINOR}")
set(NETWORKZ_VERSION_PATCH "${networkz_VERSION_PATCH}")

if(NETWORKZ_VERSION_MAJOR STREQUAL ""
   OR NETWORKZ_VERSION_MINOR STREQUAL ""
   OR NETWORKZ_VERSION_PATCH STREQUAL "")
  message(FATAL_ERROR "Failed to determine networkz version from '${NETWORKZ_VERSION}'")
endif()

message(STATUS "networkz version: "
               "${NETWORKZ_VERSION_MAJOR}.${NETWORKZ_VERSION_MINOR}.${NETWORKZ_VERSION_PATCH} "
               "(full: '${NETWORKZ_VERSION}')")

set(NETWORKZ_SOURCE_DIR ${PROJECT_SOURCE_DIR})
set(NETWORKZ_BINARY_DIR ${PROJECT_BINARY_DIR})

# ----- [ LOCAL modules ] ------
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules")

include(CMakePackageConfigHelpers)
include(CMakeParseArguments)
include(ExternalProject)
include(FindPackageHandleStandardArgs)

set(NETWORKZ_CMAKE_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")
set(NETWORKZ_DOC_DIR "share/doc/${PROJECT_NAME}")

# ------ [ CMAKE OPTIONS ] -----
include(DefineOptions)

# To accelerate compilations
if(NETWORKZ_USE_CCACHE)
  find_program(CCACHE_FOUND ccache)
  if(CCACHE_FOUND)
    message(STATUS "Using ccache: ${CCACHE_FOUND}")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ${CCACHE_FOUND})
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ${CCACHE_FOUND})
    # let ccache preserve C++ comments, because some of them may be
    # meaningful to the compiler
    set(ENV{CCACHE_COMMENTS} "1")
  endif(CCACHE_FOUND)
endif()

if(NETWORKZ_USE_PRECOMPILED_HEADERS AND ${CMAKE_VERSION} VERSION_LESS "3.16.0")
  message(WARNING "Precompiled headers need CMake 3.16.0 or later, disabling")
  set(NETWORKZ_USE_PRECOMPILED_HEADERS OFF)
endif()

if(NETWORKZ_OPTIONAL_INSTALL)
  # Don't make the "install" target depend on the "all" target
  set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY true)
  set(INSTALL_IS_OPTIONAL OPTIONAL)
endif()

# ----- [ Compiler flags ] -----
include(SetupCxxFlags)

# ----- [ Build output directory ] -----
# set compile output directory
string(TOLOWER ${CMAKE_BUILD_TYPE} BUILD_SUBDIR_NAME)

# If build in-source, create the latest symlink. If build out-of-source, which is
# preferred, simply output the binaries in the build folder
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_BINARY_DIR})
  set(BUILD_OUTPUT_ROOT_DIRECTORY
      "${CMAKE_CURRENT_BINARY_DIR}/build/${BUILD_SUBDIR_NAME}/")
  # Link build/latest to the current build directory, to avoid developers
  # accidentally running the latest debug build when in fact they're building
  # release builds.
  file(MAKE_DIRECTORY ${BUILD_OUTPUT_ROOT_DIRECTORY})
  if(NOT APPLE)
    set(MORE_ARGS "-T")
  endif()
  execute_process(COMMAND ln
                          ${MORE_ARGS}
                          -sf
                          ${BUILD_OUTPUT_ROOT_DIRECTORY}
                          ${CMAKE_CURRENT_BINARY_DIR}/build/latest)
else()
  set(BUILD_OUTPUT_ROOT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${BUILD_SUBDIR_NAME}/")
endif()

# where to put generated binaries
set(EXECUTABLE_OUTPUT_PATH "${BUILD_OUTPUT_ROOT_DIRECTORY}")

if(CMAKE_GENERATOR STREQUAL Xcode)
  # Xcode projects support multi-configuration builds.  This forces a single output directory
  # when building with Xcode that is consistent with single-configuration Makefile driven build.
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${UPPERCASE_BUILD_TYPE}
      "${BUILD_OUTPUT_ROOT_DIRECTORY}")
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${UPPERCASE_BUILD_TYPE}
      "${BUILD_OUTPUT_ROOT_DIRECTORY}")
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${UPPERCASE_BUILD_TYPE}
      "${BUILD_OUTPUT_ROOT_DIRECTORY}")
endif()

# ----- [ Dependencies ] -----
include(BuildUtils)
enable_testing()

include(ThirdPartyToolchain)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_COMMON_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${NETWORKZ_CXXFLAGS}")

# For any C code, use the same flags. These flags don't contain
# C++ specific flags.
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CXX_COMMON_FLAGS} ${NETWORKZ_CXXFLAGS}")

# Remove --std=c++17 to avoid errors from C compilers
string(REPLACE "-std=c++17" "" CMAKE_C_FLAGS ${CMAKE_C_FLAGS})

# Add C++-only flags, like -std=c++17
set(CMAKE_CXX_FLAGS "${CXX_ONLY_FLAGS} ${CMAKE_CXX_FLAGS}")

# CMAKE_CXX_FLAGS now fully assembled
message(STATUS "CMAKE_C_FLAGS: ${CMAKE_C_FLAGS}")
message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")

# Add location of Header Files
set(NETWORKZ_HEADERS_DIR
  ${PROJECT_SOURCE_DIR}
  #${EIGEN3_INCLUDE_DIR}
  #${Boost_INCLUDE_DIRS}
)

# ----- [ make ctags target ] -----
if(UNIX)
  add_custom_target(ctags ctags -R --languages=c++,c)
endif(UNIX)

# ----- [ make etags target ] -----
if(UNIX)
  add_custom_target(tags
                    etags
                    --members
                    --declarations
                    `find
                    ${CMAKE_CURRENT_SOURCE_DIR}/src
                    -name
                    \\*.cc
                    -or
                    -name
                    \\*.hh
                    -or
                    -name
                    \\*.cpp
                    -or
                    -name
                    \\*.h
                    -or
                    -name
                    \\*.c
                    -or
                    -name
                    \\*.f`)
  add_custom_target(etags DEPENDS tags)
endif(UNIX)

# ----- [ make cscope target ] -----
if(UNIX)
  add_custom_target(cscope find ${CMAKE_CURRENT_SOURCE_DIR}
                    (-name
                     \\*.cc
                     -or
                     -name
                     \\*.hh
                     -or
                     -name
                     \\*.cpp
                     -or
                     -name
                     \\*.h
                     -or
                     -name
                     \\*.c
                     -or
                     -name
                     \\*.f)
                    -exec
                    echo
                    \"{}\"
                    \;
                    >
                    cscope.files
                    &&
                    cscope
                    -q
                    -b
                    VERBATIM)
endif(UNIX)

# ----- [ Linker and Dependencies ] -----
set(NETWORKZ_LINK_LIBS)
set(NETWORKZ_STATIC_LINK_LIBS)
set(NETWORKZ_STATIC_INSTALL_INTERFACE_LIBS)

if(NETWORKZ_WITH_ZLIB)
  list(APPEND NETWORKZ_STATIC_LINK_LIBS ZLIB::ZLIB)
  if(ZLIB_SOURCE STREQUAL "SYSTEM")
    list(APPEND NETWORKZ_STATIC_INSTALL_INTERFACE_LIBS ZLIB::ZLIB)
  endif()
endif()

add_custom_target(networkz_dependencies)

if(NETWORKZ_STATIC_LINK_LIBS)
  add_dependencies(networkz_dependencies ${NETWORKZ_STATIC_LINK_LIBS})
  if(NETWORKZ_ORC)
    if(NOT MSVC_TOOLCHAIN)
      list(APPEND NETWORKZ_STATIC_LINK_LIBS ${CMAKE_DL_LIBS})
      list(APPEND NETWORKZ_STATIC_INSTALL_INTERFACE_LIBS ${CMAKE_DL_LIBS})
    endif()
  endif()
endif()

set(NETWORKZ_SHARED_PRIVATE_LINK_LIBS ${NETWORKZ_STATIC_LINK_LIBS})

if(NETWORKZ_JEMALLOC)
  add_definitions(-DNETWORKZ_JEMALLOC)
  add_definitions(-DNETWORKZ_JEMALLOC_INCLUDE_DIR=${JEMALLOC_INCLUDE_DIR})
  list(APPEND NETWORKZ_LINK_LIBS jemalloc::jemalloc)
  list(APPEND NETWORKZ_STATIC_LINK_LIBS jemalloc::jemalloc)
endif()

# ----- [ Platform-related libraries ] -----
set(NETWORKZ_SYSTEM_LINK_LIBS)

if(THREADS_FOUND)
  list(APPEND NETWORKZ_SYSTEM_LINK_LIBS Threads::Threads)
endif()

if(WIN32)
  # Winsock
  list(APPEND NETWORKZ_SYSTEM_LINK_LIBS "ws2_32.dll")
endif()

if(NOT WIN32 AND NOT APPLE)
  # Pass -lrt on Linux only
  list(APPEND NETWORKZ_SYSTEM_LINK_LIBS rt)
endif()

list(APPEND NETWORKZ_LINK_LIBS ${NETWORKZ_SYSTEM_LINK_LIBS})
list(APPEND NETWORKZ_STATIC_LINK_LIBS ${NETWORKZ_SYSTEM_LINK_LIBS})
list(APPEND NETWORKZ_STATIC_INSTALL_INTERFACE_LIBS ${NETOWRKZ_SYSTEM_LINK_LIBS})

# ----- [ Subdirectories ] -----

# --- C++ source code ---
# Add main.cpp file of project root directory as source file
set(NETWORKZ_SRCS
  sysutil.cpp
  dataframe.cpp
  IFileReaderFuncs.cpp
  FileReaderBase.cpp
  TSVReader.cpp
  graph.cpp
  graph_report.cpp
  graph_util.cpp
  graph_algo.cpp
  csr_graph.cpp
  modularity.cpp
  community_detection.cpp
  utility.cpp
  main.cpp
)

# --- Add include headers ---
include_directories(${NETWORKZ_HEADERS_DIR})

# --- Add source code files ---
set(SOURCE_FILES
  ${NETWORKZ_SRCS}
)

# --- Create the executable target ---
#  with source files listed in SOURCE_FILES variable.
add_executable(networkz 
  ${SOURCE_FILES}
)

message( " NETWORKZ_SYSTEM_DEPENDENCIES: ${NETWORKZ_SYSTEM_DEPENDENCIES}")
message( " NETWORKZ_THIRDPARTY_DEPENDENCIES: ${NETWORKZ_THIRDPARTY_DEPENDENCIES}")
message( " NETWORKZ_LINK_LIBS: ${NETWORKZ_LINK_LIBS}")
# --- Link to the library ---
target_link_libraries(networkz
   # ${Boost_LIBRARIES}
   # Eigen3::Eigen
   ${NETWORKZ_SYSTEM_DEPENDENCIES}
   ${NETWORKZ_THIRDPARTY_DEPENDENCIES}
   ${NETWORKZ_LINK_LIBS}
)

# --- Generate API docs by Oxygen ---
if(NETWORTKZ_BUILD_DOC)
  # --- doc generator packages ---
  # Doxygen to generate API document
  find_package(Doxygen)
  if(DOXYGEN_FOUND)
     # set input and output files
     set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/../docs/Doxyfile.in)
     set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

     # request to configure the file
      configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
      message("Doxygen build started")

      # note the option ALL which allows to build the docs together with the application
      add_custom_target( doc_doxygen ALL
          COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
          WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
          COMMENT "Generating API documentation with Doxygen"
          VERBATIM )
  else(DOXYGEN_FOUND)
    message("Doxygen need to be installed to generate the doxygen documentation")
  endif(DOXYGEN_FOUND)
endif(NETWORTKZ_BUILD_DOC)

# ----- [ Validate and print out configuration options ] -----
validate_config()
config_summary_message()

# ------ [ BOOST ] ------
# BOOST 1.74.0, EIGEN 3.3.9
#find_package( Boost REQUIRED COMPONENTS program_options graph iostreams timer filesystem )
#include_directories( ${Boost_INCLUDE_DIRS} )

# ------ [ EIGEN self-contained version ] ------
#find_package (Eigen3 3.3.9 REQUIRED)

# ----- [ ClangTools and InferTools]
find_package(ClangTools)
find_package(InferTools)
if("$ENV{CMAKE_EXPORT_COMPILE_COMMANDS}" STREQUAL "1" OR CLANG_TIDY_FOUND OR INFER_FOUND)
  # Generate a Clang compile_commands.json "compilation database" file for use
  # with various development tools, such as Vim's YouCompleteMe plugin.
  # See http://clang.llvm.org/docs/JSONCompilationDatabase.html
  set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
endif()
