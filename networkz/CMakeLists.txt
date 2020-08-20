cmake_minimum_required(VERSION 3.13)  # CMake version check
project(networkz C CXX)               # Create project "networkz"

set(CMAKE_CXX_STANDARD 17)      # Enable c++17 standard

# --- Networkz version ---
# The project version number.
set(VERSION_MAJOR   1   CACHE STRING "networkz major version number.")
set(VERSION_MINOR   0   CACHE STRING "networkz minor version number.")
set(VERSION_PATCH   0   CACHE STRING "networkz patch version number.")

# --- Required packages ---
# Add self-contained eigen3
include(${CMAKE_SOURCE_DIR}/thirdparty/eigen/share/eigen3/cmake/Eigen3Config.cmake)

# ------ [ BOOST ] ------
# BOOST 1.72,0 or later, EIGEN 3.3.90 or later
find_package( Boost REQUIRED COMPONENTS program_options graph iostreams timer )
include_directories( ${Boost_INCLUDE_DIRS} )

# ------ [ EIGEN self-contained version ] ------
find_package (Eigen3 3.3.90 REQUIRED)

# --- Compiler options ---
set(CMAKE_CXX_FLAGS "-fdeclspec")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-fvisibility=hidden -Os")

# --- Include Header Files ---
include_directories(${PROJECT_SOURCE_DIR})
# --- Compile c++ code ---
# Add main.cpp file of project root directory as source file
set(SOURCE_FILES
  dataframe.cpp
  IFileReaderFuncs.cpp
  FileReaderBase.cpp
  TSVReader.cpp
  graph.cpp
  graph_report.cpp
  utility.cpp
  main.cpp
)

# --- Create the executable target ---
#  with source files listed in SOURCE_FILES variable.
add_executable(networkz ${SOURCE_FILES})

# --- Link to the library ---
target_link_libraries(networkz
   ${Boost_LIBRARIES}
   Eigen3::Eigen
)
