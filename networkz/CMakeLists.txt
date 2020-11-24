cmake_minimum_required(VERSION 3.13)  # CMake version check
project(networkz C CXX)               # Create project "networkz"

set(CMAKE_CXX_STANDARD 17)      # Enable c++17 standard

# --- Networkz version ---
# The project version number.
set(VERSION_MAJOR   1   CACHE STRING "networkz major version number.")
set(VERSION_MINOR   0   CACHE STRING "networkz minor version number.")
set(VERSION_PATCH   0   CACHE STRING "networkz patch version number.")

# --- Required packages ---
# Doxygen to generate API document
find_package(Doxygen)

# Add self-contained eigen3
include(${CMAKE_SOURCE_DIR}/thirdparty/eigen/share/eigen3/cmake/Eigen3Config.cmake)

# ------ [ BOOST ] ------
# BOOST 1.72,0 or later, EIGEN 3.3.90 or later
find_package( Boost REQUIRED COMPONENTS program_options graph iostreams timer filesystem )
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
  graph_util.cpp
  graph_algo.cpp
  utility.cpp
  community_detection.cpp
  modularity.cpp
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

# --- Generate API docs by Oxygen ---
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
