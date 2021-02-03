#
# File: ThirdpartyToolchain.cmake
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

include(ProcessorCount)
processorcount(NPROC)

add_custom_target(toolchain)

# Accumulate all bundled targets and we will splice them together later as
# libarrow_dependencies.a so that third party libraries have something usable
# to create statically-linked builds with some BUNDLED dependencies, including
# allocators like jemalloc and mimalloc
set(NETWORKZ_BUNDLED_STATIC_LIBS)

# Accumulate all system dependencies to provide suitable static link
# parameters to the third party libraries.
set(NETWORKZ_SYSTEM_DEPENDENCIES)

# ----------------------------------------------------------------------
# Resolve the dependencies

set(NETWORKZ_THIRDPARTY_DEPENDENCIES
    Boost
    Eigen3
)

# For backward compatibility. We use "BOOST_SOURCE" if "Boost_SOURCE"
# isn't specified and "BOOST_SOURCE" is specified.
# We renamed "BOOST" dependency name to "Boost" in 3.0.0 because
# upstreams (CMake and Boost) use "Boost" not "BOOST" as package name.
if("${Boost_SOURCE}" STREQUAL "" AND NOT "${BOOST_SOURCE}" STREQUAL "")
  set(Boost_SOURCE ${BOOST_SOURCE})
endif()

message(STATUS "Using NETWORKZ_THIRDPARTY_DEPENDENCIES approach to find dependencies")

# For each dependency, set dependency source to global default, if unset
foreach(DEPENDENCY ${NETWORKZ_THIRDPARTY_DEPENDENCIES})
  if("${${DEPENDENCY}_SOURCE}" STREQUAL "")
    set(${DEPENDENCY}_SOURCE ${NETWORKZ_ACTUAL_DEPENDENCY_SOURCE})
    # If no ROOT was supplied and we have a global prefix, use it
    if(NOT ${DEPENDENCY}_ROOT AND NETWORKZ_PACKAGE_PREFIX)
      set(${DEPENDENCY}_ROOT ${NETWORKZ_PACKAGE_PREFIX})
    endif()
  endif()
endforeach()

# ---------- [ MACRO: build_dependency ] ----------
macro(build_dependency DEPENDENCY_NAME)
  if("${DEPENDENCY_NAME}" STREQUAL "Boost")
    build_boost()
  elseif("${DEPENDENCY_NAME}" STREQUAL "Eigen3")
    build_eigen()
  else()
    message(FATAL_ERROR "Unknown thirdparty dependency to build: ${DEPENDENCY_NAME}")
  endif()
endmacro()

# ---------- [ MACRO: provide_find_module ] ----------

# Find modules are needed by the consumer in case of a static build, or if the
# linkage is PUBLIC or INTERFACE.
macro(provide_find_module PACKAGE_NAME)
  set(module_ "${CMAKE_SOURCE_DIR}/cmake_modules/Find${PACKAGE_NAME}.cmake")
  if(EXISTS "${module_}")
    message(STATUS "Providing CMake module for ${PACKAGE_NAME}")
    install(FILES "${module_}" DESTINATION "${NETWORKZ_CMAKE_INSTALL_DIR}")
  endif()
  unset(module_)
endmacro()

# ----------- [ MACRO: resolve_dependency ] ---------
macro(resolve_dependency DEPENDENCY_NAME)
  set(options)
  set(one_value_args HAVE_ALT IS_RUNTIME_DEPENDENCY REQUIRED_VERSION USE_CONFIG)
  cmake_parse_arguments(ARG
                        "${options}"
                        "${one_value_args}"
                        "${multi_value_args}"
                        ${ARGN})
  if(ARG_UNPARSED_ARGUMENTS)
    message(SEND_ERROR "Error: unrecognized arguments: ${ARG_UNPARSED_ARGUMENTS}")
  endif()
  if("${ARG_IS_RUNTIME_DEPENDENCY}" STREQUAL "")
    set(ARG_IS_RUNTIME_DEPENDENCY TRUE)
  endif()

  if(ARG_HAVE_ALT)
    set(PACKAGE_NAME "${DEPENDENCY_NAME}Alt")
  else()
    set(PACKAGE_NAME ${DEPENDENCY_NAME})
  endif()
  set(FIND_PACKAGE_ARGUMENTS ${PACKAGE_NAME})
  if(ARG_REQUIRED_VERSION)
    list(APPEND FIND_PACKAGE_ARGUMENTS ${ARG_REQUIRED_VERSION})
  endif()
  if(ARG_USE_CONFIG)
    list(APPEND FIND_PACKAGE_ARGUMENTS CONFIG)
  endif()

  if(${DEPENDENCY_NAME}_SOURCE STREQUAL "AUTO")
    find_package(${FIND_PACKAGE_ARGUMENTS})
    # CMake 3.2 does uppercase the FOUND variable
    string(TOUPPER "${PACKAGE_NAME}_FOUND" UPPERCASE_FOUND_VARIABLE)
    if(${${PACKAGE_NAME}_FOUND} OR "${${UPPERCASE_FOUND_VARIABLE}}")
      set(${DEPENDENCY_NAME}_SOURCE "SYSTEM")
    else()
      build_dependency(${DEPENDENCY_NAME})
      set(${DEPENDENCY_NAME}_SOURCE "BUNDLED")
    endif()
  elseif(${DEPENDENCY_NAME}_SOURCE STREQUAL "BUNDLED")
    build_dependency(${DEPENDENCY_NAME})
  elseif(${DEPENDENCY_NAME}_SOURCE STREQUAL "SYSTEM")
    find_package(${FIND_PACKAGE_ARGUMENTS} REQUIRED)
  endif()
  if(${DEPENDENCY_NAME}_SOURCE STREQUAL "SYSTEM" AND ARG_IS_RUNTIME_DEPENDENCY)
    provide_find_module(${PACKAGE_NAME})
    list(APPEND NETWORKZ_SYSTEM_DEPENDENCIES ${PACKAGE_NAME})
  endif()
endmacro()

# ----------------------------------------------------------------------
# Thirdparty versions, environment variables, source URLs

set(THIRDPARTY_DIR "${networkz_SOURCE_DIR}/thirdparty")

# ----------------------------------------------------------------------
# Some EP's require other EP's

if(NETWORKZ_THRIFT)
  set(NETWORKZ_WITH_ZLIB ON)
endif()

# ----------- [ MACRO: set_urls ] ---------
# Versions and URLs for toolchain builds, which also can be used to configure
# offline builds
# Note: We should not use the Apache dist server for build dependencies

macro(set_urls URLS)
  set(${URLS} ${ARGN})
  if(CMAKE_VERSION VERSION_LESS 3.7)
    # ExternalProject doesn't support backup URLs;
    # Feature only available starting in 3.7
    list(GET ${URLS} 0 ${URLS})
  endif()
endmacro()

# Read toolchain versions from cpp/thirdparty/versions.txt
file(STRINGS "${THIRDPARTY_DIR}/versions.txt" TOOLCHAIN_VERSIONS_TXT)
foreach(_VERSION_ENTRY ${TOOLCHAIN_VERSIONS_TXT})
  # Exclude comments
  if(NOT
     ((_VERSION_ENTRY MATCHES "^[^#][A-Za-z0-9-_]+_VERSION=")
      OR (_VERSION_ENTRY MATCHES "^[^#][A-Za-z0-9-_]+_CHECKSUM=")))
    continue()
  endif()

  string(REGEX MATCH "^[^=]*" _VARIABLE_NAME ${_VERSION_ENTRY})
  string(REPLACE "${_VARIABLE_NAME}=" "" _VARIABLE_VALUE ${_VERSION_ENTRY})

  # Skip blank or malformed lines
  if(_VARIABLE_VALUE STREQUAL "")
    continue()
  endif()

  # For debugging
  message(STATUS "${_VARIABLE_NAME}: ${_VARIABLE_VALUE}")

  set(${_VARIABLE_NAME} ${_VARIABLE_VALUE})
endforeach()

if(DEFINED ENV{NETWORKZ_BOOST_URL})
  set(BOOST_SOURCE_URL "$ENV{NETWORKZ_BOOST_URL}")
else()
  string(REPLACE "." "_" NETWORKZ_BOOST_BUILD_VERSION_UNDERSCORES
                 ${NETWORKZ_BOOST_BUILD_VERSION})
  set_urls(
    BOOST_SOURCE_URL
    # These are trimmed boost bundles we maintain.
    # See cpp/build-support/trim-boost.sh
    #"https://dl.bintray.com/boostorg/release/1.74.0/source/boost_1_74_0.tar.gz"
    "https://github.com/boostorg/boost/archive/boost-${NETWORKZ_BOOST_BUILD_VERSION}.tar.gz"
    #"https://dl.bintray.com/boostorg/release/${NETWORKZ_BOOST_BUILD_VERSION}/source/boost_${NETWORKZ_BOOST_BUILD_VERSION}.tar.gz"
    #"https://dl.bintray.com/ursalabs/NETWORKZ-boost/boost_${NETWORKZ_BOOST_BUILD_VERSION_UNDERSCORES}.tar.gz"
    #"https://dl.bintray.com/boostorg/release/${NETWORKZ_BOOST_BUILD_VERSION_UNDERSCORES}/source/boost_${NETWORKZ_BOOST_BUILD_VERSION_UNDERSCORES}.tar.gz"
    #"https://github.com/boostorg/boost/archive/boost-${NETWORKZ_BOOST_BUILD_VERSION_UNDERSCORES}.tar.gz"
  )
endif()

if(DEFINED ENV{NETWORKZ_EIGEN3_URL})
  set(EIGEN3_SOURCE_URL "$ENV{NETWORKZ_EIGEN3_URL}")
else()
  set_urls(
    EIGEN3_SOURCE_URL
    "https://gitlab.com/libeigen/eigen/-/archive/${NETWORKZ_EIGEN3_BUILD_VERSION}/eigen-${NETWORKZ_EIGEN3_BUILD_VERSION}.tar.gz"
    # "https://gitlab.com/libeigen/eigen/-/archive/${NETWORKZ_EIGEN3_BUILD_VERSION}/eigen-${NETWORKZ_EIGEN3_BUILD_VERSION}.tar.bz2"
  )
endif()

if(DEFINED ENV{NETWORKZ_JEMALLOC_URL})
  set(JEMALLOC_SOURCE_URL "$ENV{NETWORKZ_JEMALLOC_URL}")
else()
  set_urls(
    JEMALLOC_SOURCE_URL
    #"https://github.com/jemalloc/jemalloc/releases/download/${NETWORKZ_JEMALLOC_BUILD_VERSION}/jemalloc-${NETWORKZ_JEMALLOC_BUILD_VERSION}.tar.bz2"
    #"https://github.com/ursa-labs/thirdparty/releases/download/latest/jemalloc-${NETWORKZ_JEMALLOC_BUILD_VERSION}.tar.bz2"
    "https://github.com/jemalloc/jemalloc/releases/download/${NETWORKZ_JEMALLOC_BUILD_VERSION}/jemalloc-${NETWORKZ_JEMALLOC_BUILD_VERSION}.tar.bz2"
    )
endif()

if(DEFINED ENV{NETWORKZ_ZLIB_URL})
  set(ZLIB_SOURCE_URL "$ENV{NETWORKZ_ZLIB_URL}")
else()
  set_urls(
    ZLIB_SOURCE_URL "https://zlib.net/fossils/zlib-${NETWORKZ_ZLIB_BUILD_VERSION}.tar.gz"
    "https://github.com/ursa-labs/thirdparty/releases/download/latest/zlib-${NETWORKZ_ZLIB_BUILD_VERSION}.tar.gz"
    )
endif()

# --------- [ ExternalProject options ]----------------------------------

set(EP_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_${UPPERCASE_BUILD_TYPE}}")
set(EP_C_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_${UPPERCASE_BUILD_TYPE}}")

if(NOT MSVC_TOOLCHAIN)
  # Set -fPIC on all external projects
  set(EP_CXX_FLAGS "${EP_CXX_FLAGS} -fPIC")
  set(EP_C_FLAGS "${EP_C_FLAGS} -fPIC")
endif()

# CC/CXX environment variables are captured on the first invocation of the
# builder (e.g make or ninja) instead of when CMake is invoked into to build
# directory. This leads to issues if the variables are exported in a subshell
# and the invocation of make/ninja is in distinct subshell without the same
# environment (CC/CXX).
set(EP_COMMON_TOOLCHAIN -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
                        -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER})

if(CMAKE_AR)
  set(EP_COMMON_TOOLCHAIN ${EP_COMMON_TOOLCHAIN} -DCMAKE_AR=${CMAKE_AR})
endif()

if(CMAKE_RANLIB)
  set(EP_COMMON_TOOLCHAIN ${EP_COMMON_TOOLCHAIN} -DCMAKE_RANLIB=${CMAKE_RANLIB})
endif()

# External projects are still able to override the following declarations.
# cmake command line will favor the last defined variable when a duplicate is
# encountered. This requires that `EP_COMMON_CMAKE_ARGS` is always the first
# argument.
set(EP_COMMON_CMAKE_ARGS
    ${EP_COMMON_TOOLCHAIN}
    ${EP_COMMON_CMAKE_ARGS}
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DCMAKE_C_FLAGS=${EP_C_FLAGS}
    -DCMAKE_C_FLAGS_${UPPERCASE_BUILD_TYPE}=${EP_C_FLAGS}
    -DCMAKE_CXX_FLAGS=${EP_CXX_FLAGS}
    -DCMAKE_CXX_FLAGS_${UPPERCASE_BUILD_TYPE}=${EP_CXX_FLAGS}
    -DCMAKE_EXPORT_NO_PACKAGE_REGISTRY=${CMAKE_EXPORT_NO_PACKAGE_REGISTRY}
    -DCMAKE_FIND_PACKAGE_NO_PACKAGE_REGISTRY=${CMAKE_FIND_PACKAGE_NO_PACKAGE_REGISTRY})

if(NOT NETWORKZ_VERBOSE_THIRDPARTY_BUILD)
  set(EP_LOG_OPTIONS
      LOG_CONFIGURE
      1
      LOG_BUILD
      1
      LOG_INSTALL
      1
      LOG_DOWNLOAD
      1
      LOG_OUTPUT_ON_FAILURE
      1)
  set(Boost_DEBUG FALSE)
else()
  set(EP_LOG_OPTIONS)
  set(Boost_DEBUG TRUE)
endif()

# Ensure that a default make is set
if("${MAKE}" STREQUAL "")
  if(NOT MSVC)
    find_program(MAKE make)
  endif()
endif()

# Using make -j in sub-make is fragile
# see discussion https://github.com/apache/NETWORKZ/pull/2779
if(${CMAKE_GENERATOR} MATCHES "Makefiles")
  set(MAKE_BUILD_ARGS "")
else()
  # limit the maximum number of jobs for ninja
  set(MAKE_BUILD_ARGS "-j${NPROC}")
endif()

# ----------------------------------------------------------------------
# Find pthreads

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# ----------- [ MACRO: build_boost ] ---------
# Add Boost dependencies (code adapted from Apache Kudu)

macro(build_boost)
  set(BOOST_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/boost_ep-prefix/src/boost_ep")
  message(status " BOOST_SOURCE_URL:${BOOST_SOURCE_URL}")
  set(BOOST_ROOT ${BOOST_PREFIX})
  set(NETWORKZ_BOOST_REQUIRE_LIBRARY ON)

  if(NETWORKZ_BOOST_REQUIRE_LIBRARY)
    set(BOOST_LIB_DIR "${BOOST_PREFIX}/stage/lib")
    set(BOOST_BUILD_LINK "static")
    if("${CMAKE_BUILD_TYPE}" STREQUAL "DEBUG")
      set(BOOST_BUILD_VARIANT "debug")
    else()
      set(BOOST_BUILD_VARIANT "release")
    endif()
    if(MSVC)
      set(BOOST_CONFIGURE_COMMAND ".\\\\bootstrap.bat")
    else()
      set(BOOST_CONFIGURE_COMMAND "./bootstrap.sh")
    endif()

    set(BOOST_BUILD_WITH_LIBRARIES "filesystem" "regex" "system" "program_options" "timer" "iostreams")
    string(REPLACE ";" "," BOOST_CONFIGURE_LIBRARIES "${BOOST_BUILD_WITH_LIBRARIES}")
    list(APPEND BOOST_CONFIGURE_COMMAND "--prefix=${BOOST_PREFIX}"
                "--with-libraries=${BOOST_CONFIGURE_LIBRARIES}")
    set(BOOST_BUILD_COMMAND "./b2" "-j${NPROC}" "link=${BOOST_BUILD_LINK}"
                            "variant=${BOOST_BUILD_VARIANT}")
    if(MSVC)
      string(REGEX
             REPLACE "([0-9])$" ".\\1" BOOST_TOOLSET_MSVC_VERSION ${MSVC_TOOLSET_VERSION})
      list(APPEND BOOST_BUILD_COMMAND "toolset=msvc-${BOOST_TOOLSET_MSVC_VERSION}")
      set(BOOST_BUILD_WITH_LIBRARIES_MSVC)
      foreach(_BOOST_LIB ${BOOST_BUILD_WITH_LIBRARIES})
        list(APPEND BOOST_BUILD_WITH_LIBRARIES_MSVC "--with-${_BOOST_LIB}")
      endforeach()
      list(APPEND BOOST_BUILD_COMMAND ${BOOST_BUILD_WITH_LIBRARIES_MSVC})
    else()
      list(APPEND BOOST_BUILD_COMMAND "cxxflags=-fPIC")
    endif()

    if(MSVC)
      string(REGEX
             REPLACE "^([0-9]+)\\.([0-9]+)\\.[0-9]+$" "\\1_\\2"
                     NETWORKZ_BOOST_BUILD_VERSION_NO_MICRO_UNDERSCORE
                     ${NETWORKZ_BOOST_BUILD_VERSION})
      set(BOOST_LIBRARY_SUFFIX "-vc${MSVC_TOOLSET_VERSION}-mt")
      if(BOOST_BUILD_VARIANT STREQUAL "debug")
        set(BOOST_LIBRARY_SUFFIX "${BOOST_LIBRARY_SUFFIX}-gd")
      endif()
      set(BOOST_LIBRARY_SUFFIX
          "${BOOST_LIBRARY_SUFFIX}-x64-${NETWORKZ_BOOST_BUILD_VERSION_NO_MICRO_UNDERSCORE}")
    else()
      set(BOOST_LIBRARY_SUFFIX "")
    endif()
    set(
      BOOST_STATIC_SYSTEM_LIBRARY
      "${BOOST_LIB_DIR}/libboost_system${BOOST_LIBRARY_SUFFIX}${CMAKE_STATIC_LIBRARY_SUFFIX}"
      )
    set(
      BOOST_STATIC_FILESYSTEM_LIBRARY
      "${BOOST_LIB_DIR}/libboost_filesystem${BOOST_LIBRARY_SUFFIX}${CMAKE_STATIC_LIBRARY_SUFFIX}"
      )
    set(
      BOOST_STATIC_REGEX_LIBRARY
      "${BOOST_LIB_DIR}/libboost_regex${BOOST_LIBRARY_SUFFIX}${CMAKE_STATIC_LIBRARY_SUFFIX}"
      )
    set(
      BOOST_STATIC_PROGRAM_OPTIONS_LIBRARY
      "${BOOST_LIB_DIR}/libboost_iostreams${BOOST_LIBRARY_SUFFIX}${CMAKE_STATIC_LIBRARY_SUFFIX}"
      )
    set(
      BOOST_STATIC_IOSTREAMS_LIBRARY
      "${BOOST_LIB_DIR}/libboost_graph${BOOST_LIBRARY_SUFFIX}${CMAKE_STATIC_LIBRARY_SUFFIX}"
      )
    set(
      BOOST_STATIC_TIMER_LIBRARY
      "${BOOST_LIB_DIR}/libboost_timer${BOOST_LIBRARY_SUFFIX}${CMAKE_STATIC_LIBRARY_SUFFIX}"
      )
    set(BOOST_SYSTEM_LIBRARY boost_system_static)
    set(BOOST_FILESYSTEM_LIBRARY boost_filesystem_static)
    set(BOOST_REGEX_LIBRARY boost_regex_static)
    set(BOOST_STATIC_PROGRAM_OPTIONS_LIBRARY boost_program_options)
    set(BOOST_STATIC_IOSTREAMS_LIBRARY boost_iostreams)
    set(BOOST_STATIC_TIMER_LIBRARY boost_timer)
    set(BOOST_BUILD_PRODUCTS ${BOOST_STATIC_SYSTEM_LIBRARY}
                             ${BOOST_STATIC_FILESYSTEM_LIBRARY}
                             ${BOOST_STATIC_REGEX_LIBRARY}
                             ${BOOST_STATIC_PROGRAM_OPTIONS_LIBRARY}
                             ${BOOST_STATIC_IOSTREAMS_LIBRARY}
                             ${BOOST_STATIC_TIMER_LIBRARY}
                             )

    add_thirdparty_lib(boost_system STATIC_LIB "${BOOST_STATIC_SYSTEM_LIBRARY}")

    add_thirdparty_lib(boost_filesystem STATIC_LIB "${BOOST_STATIC_FILESYSTEM_LIBRARY}")

    add_thirdparty_lib(boost_regex STATIC_LIB "${BOOST_STATIC_REGEX_LIBRARY}")
    
    add_thirdparty_lib(boost_program_options STATIC_LIB "${BOOST_STATIC_PROGRAM_OPTIONS_LIBRARY}")
    
    add_thirdparty_lib(boost_iostreams STATIC_LIB "${BOOST_STATIC_IOSTREAMS_LIBRARY}")
    
    add_thirdparty_lib(boost_timer STATIC_LIB "${BOOST_STATIC_TIMER_LIBRARY}")

    externalproject_add(
      boost_ep
      URL ${BOOST_SOURCE_URL}
      URL_HASH SHA256=afff36d392885120bcac079148c177d1f6f7730ec3d47233aa51b0afa4db94a5
      BUILD_BYPRODUCTS ${BOOST_BUILD_PRODUCTS}
      BUILD_IN_SOURCE 1
      CONFIGURE_COMMAND ${BOOST_CONFIGURE_COMMAND}
      BUILD_COMMAND ${BOOST_BUILD_COMMAND}
      INSTALL_COMMAND "" ${EP_LOG_OPTIONS})
    list(APPEND NETWORKZ_BUNDLED_STATIC_LIBS boost_system_static boost_filesystem_static
                boost_regex_static boost_program_options_static boost_iostreams_static boost_timer_static)
  else()
    message(status " Add external project boost")
    externalproject_add(boost_ep
                        ${EP_LOG_OPTIONS}
                        BUILD_COMMAND ""
                        CONFIGURE_COMMAND ""
                        INSTALL_COMMAND ""
                        URL ${BOOST_SOURCE_URL})
  endif()

  set(Boost_INCLUDE_DIR "${BOOST_PREFIX}")
  set(Boost_INCLUDE_DIRS "${Boost_INCLUDE_DIR}")
  add_dependencies(toolchain boost_ep)
  set(BOOST_VENDORED TRUE)

  include_directories(SYSTEM "${BOOST_ROOT}/")
  # The include directory must exist before it is referenced by a target.
  file(MAKE_DIRECTORY "${BOOST_ROOT}/")
  add_library(boost::boost STATIC IMPORTED)
  set_target_properties(boost::boost
                        PROPERTIES INTERFACE_LINK_LIBRARIES
                                   Threads::Threads
                                   IMPORTED_LOCATION
                                   "${BOOST_BUILD_PRODUCTS}"
                                   INTERFACE_INCLUDE_DIRECTORIES
                                   "${CMAKE_CURRENT_BINARY_DIR}/boost_ep-prefix/src")
  add_dependencies(boost::boost boost_ep)

  list(APPEND NETWORKZ_THIRDPARTY_DEPENDENCIES boost::boost)
endmacro()

set(Boost_USE_MULTITHREADED ON)
if(MSVC AND NETWORKZ_USE_STATIC_CRT)
  set(Boost_USE_STATIC_RUNTIME ON)
endif()

# ----------- [ MACRO: build_eigen3 ] ---------
# Eigen3

macro(build_eigen3)
  set(EIGEN3_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/eigen3_ep-prefix/src/eigen3_ep/dist")
  
  set(EIGEN3_CMAKE_ARGS "-DCMAKE_BUILD_TYPE=Release" "-DCMAKE_INSTALL_PREFIX=${EIGEN3_PREFIX}")
  set(EIGEN3_BUILD_COMMAND ${MAKE} ${MAKE_BUILD_ARGS})
  if(CMAKE_OSX_SYSROOT)
    list(APPEND EIGEN3_CMAKE_ARGS "-DCMAKE_OSX_SYSROOT=${CMAKE_OSX_SYSROOT}")
  endif()

  externalproject_add(
    eigen3_ep
    DEPENDS boost_ep
    URL ${EIGEN3_SOURCE_URL}
    URL_HASH SHA256=7985975b787340124786f092b3a07d594b2e9cd53bbfe5f3d9b1daee7d55f56f
    UPDATE_COMMAND ""
    BUILD_COMMAND ${EIGEN3_BUILD_COMMAND}
    INSTALL_COMMAND ${MAKE} -j${NPROC} install
    CMAKE_ARGS ${EIGEN3_CMAKE_ARGS}
    )

  include_directories(SYSTEM "${EIGEN3_PREFIX}/include/eigen3")
  file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/eigen3_ep-prefix/src/eigen3_ep")
  add_library(eigen3::eigen3 STATIC IMPORTED)
  #include_directories(SYSTEM ${EIGEN3_PREFIX}/include/eigen3)
  set_target_properties(eigen3::eigen3
                        PROPERTIES 
                          IMPORTED_LOCATION
                          "${EIGEN3_PREFIX}"
                          #INTERFACE_INCLUDE_DIRECTORIES
                          #"${EIGEN3_PREFIX}/include/eigen3"
                      )
  add_dependencies(eigen3::eigen3 eigen3_ep)
  list(APPEND NETWORKZ_THIRDPARTY_DEPENDENCIES eigen3::eigen3)
  list(APPEND NETWORKZ_HEADERS_DIR ${EIGEN3_PREFIX}/include/eigen3)

endmacro()

# ----------- [ MACRO: build_jemalloc ] --------------
macro(build_jemalloc)
  set(NETWORKZ_JEMALLOC_USE_SHARED OFF)
  set(JEMALLOC_PREFIX
      "${CMAKE_CURRENT_BINARY_DIR}/jemalloc_ep-prefix/src/jemalloc_ep/dist/")
  set(JEMALLOC_STATIC_LIB
      "${JEMALLOC_PREFIX}/lib/libjemalloc_pic${CMAKE_STATIC_LIBRARY_SUFFIX}")
  set(JEMALLOC_CONFIGURE_COMMAND ./configure "AR=${CMAKE_AR}" "CC=${CMAKE_C_COMPILER}")
  if(CMAKE_OSX_SYSROOT)
    list(APPEND JEMALLOC_CONFIGURE_COMMAND "SDKROOT=${CMAKE_OSX_SYSROOT}")
  endif()
  list(APPEND JEMALLOC_CONFIGURE_COMMAND
              "--prefix=${JEMALLOC_PREFIX}"
              "--with-jemalloc-prefix=je_arrow_"
              "--with-private-namespace=je_arrow_private_"
              "--without-export"
              "--disable-shared"
              # Don't override operator new()
              "--disable-cxx" "--disable-libdl"
              # See https://github.com/jemalloc/jemalloc/issues/1237
              "--disable-initial-exec-tls" ${EP_LOG_OPTIONS})
  set(JEMALLOC_BUILD_COMMAND ${MAKE} ${MAKE_BUILD_ARGS})
  if(CMAKE_OSX_SYSROOT)
    list(APPEND JEMALLOC_BUILD_COMMAND "SDKROOT=${CMAKE_OSX_SYSROOT}")
  endif()
  externalproject_add(
    jemalloc_ep
    URL ${JEMALLOC_SOURCE_URL}
    PATCH_COMMAND
      touch doc/jemalloc.3 doc/jemalloc.html
      # The prefix "je_arrow_" must be kept in sync with the value in memory_pool.cc
    CONFIGURE_COMMAND ${JEMALLOC_CONFIGURE_COMMAND}
    BUILD_IN_SOURCE 1
    BUILD_COMMAND ${JEMALLOC_BUILD_COMMAND}
    BUILD_BYPRODUCTS "${JEMALLOC_STATIC_LIB}"
    INSTALL_COMMAND ${MAKE} -j1 install)

  # Don't use the include directory directly so that we can point to a path
  # that is unique to our codebase.
  include_directories(SYSTEM "${CMAKE_CURRENT_BINARY_DIR}/jemalloc_ep-prefix/src/")
  # The include directory must exist before it is referenced by a target.
  file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/jemalloc_ep-prefix/src/")
  add_library(jemalloc::jemalloc STATIC IMPORTED)
  set_target_properties(jemalloc::jemalloc
                        PROPERTIES INTERFACE_LINK_LIBRARIES
                                   Threads::Threads
                                   IMPORTED_LOCATION
                                   "${JEMALLOC_STATIC_LIB}"
                                   INTERFACE_INCLUDE_DIRECTORIES
                                   "${CMAKE_CURRENT_BINARY_DIR}/jemalloc_ep-prefix/src")
  add_dependencies(jemalloc::jemalloc jemalloc_ep)

  list(APPEND NETWORKZ_BUNDLED_STATIC_LIBS jemalloc::jemalloc)
endmacro()

# ----------------------------------------------------------------------
# boost - build boost library
set(NETWORKZ_BOOST_REQUIRED ON)
if(NETWORKZ_BOOST_REQUIRED)
  message(STATUS "Building boost from source")
  set(NETWORKZ_BOOST_REQUIRE_LIBRARY ON)
  build_boost()
endif()

# ----------------------------------------------------------------------
# eigen3 - build EIGEN library

if(NETWORKZ_EIGEN3)
  message(STATUS "Building eigen3 from source")
  build_eigen3()
  # message(STATUS "EIGEN3 include dir: ${EIGEN3_INCLUDE_DIR}")
endif()

# ----------------------------------------------------------------------
# jemalloc - Unix-only high-performance allocator

if(NETWORKZ_JEMALLOC)
  message(STATUS "Building (vendored) jemalloc from source")
  # We only use a vendored jemalloc as we want to control its version.
  # Also our build of jemalloc is specially prefixed so that it will not
  # conflict with the default allocator as well as other jemalloc
  # installations.
  # find_package(jemalloc)
  build_jemalloc()
  
endif()