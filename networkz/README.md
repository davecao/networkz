#  Networkz

## Prerequisites

1. cmake (xcode project also provided)
2. Boost Graph Library (version 1.70 or later)
3. Eigen (version 3.3.90 or later)
4. C++ compiler (support c++17, clang 9.0 had been tested on MacOS) 

## Compilation

    mkdir build; cd build
    cmake -DCMAKE_BUILD_TYPE=Release ../path/to/networkz
    make VERBOSE=1


The executable file will be generated in the ~build~ directory.

## Format of Input file
Tab-Separated Values (TSV) format.

