cmake_minimum_required(VERSION 3.13)  # CMake version check
project(networkz C CXX)               # Create project "networkz"

set(CMAKE_CXX_STANDARD 17)      # Enable c++17 standard

# --- Networkz version ---
# The project version number.
set(VERSION_MAJOR   1   CACHE STRING "networkz major version number.")
set(VERSION_MINOR   0   CACHE STRING "networkz minor version number.")
set(VERSION_PATCH   0   CACHE STRING "networkz patch version number.")

# ------ Required packages ------

# ------ [ BOOST ] ------
# BOOST 1.72,0 or later, EIGEN 3.90 or later
find_package( Boost REQUIRED COMPONENTS program_options graph iostreams timer )
include_directories( ${Boost_INCLUDE_DIRS} )

# ------ [ EIGEN ] ------
# find_package(EIGEN REQUIRED)
find_package (Eigen3 3.3.9 NO_MODULE)
set(EIGEN3_INC "$ENV{EIGEN3_INCLUDE_DIR}")

if (NOT EIGEN3_INC)
  if (TARGET Eigen3::Eigen)
    set(EIGEN3_INC "${EIGEN3_INCLUDE_DIR}")
  else ()
    message(status " Could not find Eigen3 library")
  endif (TARGET Eigen3::Eigen)
endif (NOT EIGEN3_INC)

include_directories("${EIGEN3_INC}")
message(status " EIGEN3:" ${EIGEN3_INC})

# Include source files
include_directories(${PROJECT_SOURCE_DIR})

# Compiler options
set (CMAKE_CXX_FLAGS "-fdeclspec")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-Os")

# Add main.cpp file of project root directory as source file
set(SOURCE_FILES
  IFileReaderFuncs.cpp
  FileReaderBase.cpp
  TSVReader.cpp
  graph_auxiliary.cpp
  graph_report.cpp
  utility.cpp
  main.cpp
)

# Add executable target with source files listed in SOURCE_FILES variable
add_executable(networkz ${SOURCE_FILES})

target_link_libraries(networkz
   ${Boost_LIBRARIES}
   c++
)
