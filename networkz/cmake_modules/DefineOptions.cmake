#
# File: DefineOptions.cmake
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

# ----------- [ MACRO: set_option_category ] ---------
macro(set_option_category name)
  set(NETWORKZ_OPTION_CATEGORY ${name})
  list(APPEND "NETWORKZ_OPTION_CATEGORIES" ${name})
endmacro()

# ----------- [ FUNCTION: check_description_length ] ---------
function(check_description_length name description)
  foreach(description_line ${description})
    string(LENGTH ${description_line} line_length)
    if(${line_length} GREATER 80)
      message(FATAL_ERROR "description for ${name} contained a\n\
        line ${line_length} characters long!\n\
        (max is 80). Split it into more lines with semicolons")
    endif()
  endforeach()
endfunction()

# ----------- [ FUNCTION: list_join ] ---------
function(list_join lst glue out)
  if("${${lst}}" STREQUAL "")
    set(${out} "" PARENT_SCOPE)
    return()
  endif()

  list(GET ${lst} 0 joined)
  list(REMOVE_AT ${lst} 0)
  foreach(item ${${lst}})
    set(joined "${joined}${glue}${item}")
  endforeach()
  set(${out} ${joined} PARENT_SCOPE)
endfunction()

# ----------- [ MACRO: define_option ] ---------
macro(define_option name description default)
  check_description_length(${name} ${description})
  list_join(description "\n" multiline_description)

  option(${name} "${multiline_description}" ${default})

  list(APPEND "NETWORKZ_${NETWORKZ_OPTION_CATEGORY}_OPTION_NAMES" ${name})
  set("${name}_OPTION_DESCRIPTION" ${description})
  set("${name}_OPTION_DEFAULT" ${default})
  set("${name}_OPTION_TYPE" "bool")
endmacro()

# ----------- [ MACRO: define_option_string ] ---------
macro(define_option_string name description default)
  check_description_length(${name} ${description})
  list_join(description "\n" multiline_description)

  set(${name} ${default} CACHE STRING "${multiline_description}")

  list(APPEND "NETWORKZ_${NETWORKZ_OPTION_CATEGORY}_OPTION_NAMES" ${name})
  set("${name}_OPTION_DESCRIPTION" ${description})
  set("${name}_OPTION_DEFAULT" "\"${default}\"")
  set("${name}_OPTION_TYPE" "string")
  set("${name}_OPTION_POSSIBLE_VALUES" ${ARGN})

  list_join("${name}_OPTION_POSSIBLE_VALUES" "|" "${name}_OPTION_ENUM")
  if(NOT ("${${name}_OPTION_ENUM}" STREQUAL ""))
    set_property(CACHE ${name} PROPERTY STRINGS "${name}_OPTION_POSSIBLE_VALUES")
  endif()
endmacro()

# ----------- [ MACRO: validate_config ] ---------
macro(validate_config)
  foreach(category ${NETWORKZ_OPTION_CATEGORIES})
    set(option_names ${NETWORKZ_${category}_OPTION_NAMES})

    foreach(name ${option_names})
      set(possible_values ${${name}_OPTION_POSSIBLE_VALUES})
      set(value "${${name}}")
      if(possible_values)
        if(NOT "${value}" IN_LIST possible_values)
          message(
            FATAL_ERROR "Configuration option ${name} got invalid value '${value}'. "
                        "Allowed values: ${${name}_OPTION_ENUM}.")
        endif()
      endif()
    endforeach()

  endforeach()
endmacro()

# Top level cmake dir
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_SOURCE_DIR}")
  #----------------------------------------------------------------------
  set_option_category("Compile and link")

  define_option_string(NETWORKZ_CXXFLAGS "Compiler flags to append when compiling Networkz" "")

  define_option(NETWORKZ_BUILD_STATIC "Build static libraries" ON)

  define_option(NETWORKZ_BUILD_SHARED "Build shared libraries" ON)

  define_option_string(NETWORKZ_PACKAGE_KIND
                       "Arbitrary string that identifies the kind of package;\
                      (for informational purposes)" "")

  define_option_string(NETWORKZ_GIT_ID "The Networkz git commit id (if any)" "")

  define_option_string(NETWORKZ__GIT_DESCRIPTION "The Networkz git commit description (if any)"
                       "")

  define_option(NETWORKZ_NO_DEPRECATED_API "Exclude deprecated APIs from build" OFF)

  define_option(NETWORKZ_USE_CCACHE "Use ccache when compiling (if available)" ON)

  define_option(NETWORKZ_USE_LD_GOLD "Use ld.gold for linking on Linux (if available)" OFF)

  define_option(NETWORKZ_USE_PRECOMPILED_HEADERS "Use precompiled headers when compiling"
                OFF)

  define_option_string(NETWORKZ_SIMD_LEVEL
                       "Compile-time SIMD optimization level"
                       "SSE4_2" # default to SSE4.2
                       "NONE"
                       "SSE4_2"
                       "AVX2"
                       "AVX512")

  define_option_string(NETWORKZ_RUNTIME_SIMD_LEVEL
                       "Max runtime SIMD optimization level"
                       "MAX" # default to max supported by compiler
                       "NONE"
                       "SSE4_2"
                       "AVX2"
                       "AVX512"
                       "MAX")

  # Arm64 architectures and extensions can lead to exploding combinations.
  # So set it directly through cmake command line.
  #
  # If you change this, you need to change the definition in
  # python/CMakeLists.txt too.
  define_option_string(NETWORKZ_ARMV8_ARCH
                       "Arm64 arch and extensions"
                       "armv8-a" # Default
                       "armv8-a"
                       "armv8-a+crc+crypto")

  define_option(NETWORKZ_ALTIVEC "Build with Altivec if compiler has support" ON)

  define_option(NETWORKZ_RPATH_ORIGIN "Build networkz libraries with RATH set to \$ORIGIN" OFF)

  define_option(NETWORKZ_INSTALL_NAME_RPATH
                "Build Networkz libraries with install_name set to @rpath" ON)

  define_option(NETWORKZ_GGDB_DEBUG "Pass -ggdb flag to debug builds" ON)

  #----------------------------------------------------------------------
  set_option_category("Test and benchmark")

  define_option(NETWORKZ_BUILD_EXAMPLES "Build the Networkz examples" OFF)

  define_option(NETWORKZ_BUILD_TESTS "Build the Networkz googletest unit tests" OFF)

  define_option(NETWORKZ_ENABLE_TIMING_TESTS "Enable timing-sensitive tests" ON)

  define_option(NETWORKZ_BUILD_INTEGRATION "Build the Networkz integration test executables"
                OFF)

  define_option(NETWORKZ_BUILD_BENCHMARKS "Build the Networkz micro benchmarks" OFF)

  # Reference benchmarks are used to compare to naive implementation, or
  # discover various hardware limits.
  define_option(NETWORKZ_BUILD_BENCHMARKS_REFERENCE
                "Build the Networkz micro reference benchmarks" OFF)

  if(NETWORKZ_BUILD_SHARED)
    set(NETWORKZ_TEST_LINKAGE_DEFAULT "shared")
  else()
    set(NETWORKZ_TEST_LINKAGE_DEFAULT "static")
  endif()

  define_option_string(NETWORKZ_TEST_LINKAGE
                       "Linkage of Networkz libraries with unit tests executables."
                       "${NETWORKZ_TEST_LINKAGE_DEFAULT}"
                       "shared"
                       "static")

  define_option(NETWORKZ_FUZZING "Build Networkz Fuzzing executables" OFF)

  define_option(NETWORKZ_LARGE_MEMORY_TESTS "Enable unit tests which use large memory" OFF)

  #----------------------------------------------------------------------
  set_option_category("Lint")

  define_option(NETWORKZ_ONLY_LINT "Only define the lint and check-format targets" OFF)

  define_option(NETWORKZ_VERBOSE_LINT "If off, 'quiet' flags will be passed to linting tools"
                OFF)

  define_option(NETWORKZ_GENERATE_COVERAGE "Build with C++ code coverage enabled" OFF)

  #----------------------------------------------------------------------
  set_option_category("Checks")

  define_option(NETWORKZ_TEST_MEMCHECK "Run the test suite using valgrind --tool=memcheck"
                OFF)

  define_option(NETWORKZ_USE_ASAN "Enable Address Sanitizer checks" OFF)

  define_option(NETWORKZ_USE_TSAN "Enable Thread Sanitizer checks" OFF)

  define_option(NETWORKZ_USE_UBSAN "Enable Undefined Behavior sanitizer checks" OFF)

  #----------------------------------------------------------------------
  set_option_category("Project component")

  define_option(NETWORKZ_BUILD_UTILITIES "Build Networkz commandline utilities" OFF)

  define_option(NETWORKZ_COMPUTE "Build the Networkz Compute Modules" OFF)

  define_option(NETWORKZ_CSV "Build the Networkz CSV Parser Module" OFF)

  define_option(NETWORKZ_CUDA "Build the Networkz CUDA extensions (requires CUDA toolkit)" OFF)

  define_option(NETWORKZ_DATASET "Build the Networkz Dataset Modules" OFF)

  define_option(NETWORKZ_FILESYSTEM "Build the Networkz Filesystem Layer" OFF)

  define_option(NETWORKZ_FLIGHT
                "Build the Networkz Flight RPC System (requires GRPC, Protocol Buffers)" OFF)

  define_option(NETWORKZ_GANDIVA "Build the Gandiva libraries" OFF)

  define_option(NETWORKZ_HDFS "Build the Networkz HDFS bridge" OFF)

  define_option(NETWORKZ_HIVESERVER2 "Build the HiveServer2 client and Networkz adapter" OFF)

  define_option(NETWORKZ_IPC "Build the Networkz IPC extensions" ON)

  set(NETWORKZ_JEMALLOC_DESCRIPTION "Build the Networkz jemalloc-based allocator")
  if(WIN32 OR "${CMAKE_SYSTEM_NAME}" STREQUAL "FreeBSD")
    # jemalloc is not supported on Windows.
    #
    # jemalloc is the default malloc implementation on FreeBSD and can't
    # be built with --disable-libdl on FreeBSD. Because lazy-lock feature
    # is required on FreeBSD. Lazy-lock feature requires libdl.
    define_option(NETWORKZ_JEMALLOC ${NETWORKZ_JEMALLOC_DESCRIPTION} OFF)
  else()
    define_option(NETWORKZ_JEMALLOC ${NETWORKZ_JEMALLOC_DESCRIPTION} OFF)
  endif()
  define_option(NETWORKZ_EIGEN3 "Build the Networkz with Eigen3" ON)
  
  define_option(NETWORKZ_JNI "Build the Networkz JNI lib" OFF)

  define_option(NETWORKZ_JSON "Build Networkz with JSON support (requires RapidJSON)" OFF)

  define_option(NETWORKZ_MIMALLOC "Build the Networkz mimalloc-based allocator" OFF)

  define_option(NETWORKZ_PARQUET "Build the Parquet libraries" OFF)

  define_option(NETWORKZ_ORC "Build the Networkz ORC adapter" OFF)

  define_option(NETWORKZ_PLASMA "Build the plasma object store along with Networkz" OFF)

  define_option(NETWORKZ_PLASMA_JAVA_CLIENT "Build the plasma object store java client" OFF)

  define_option(NETWORKZ_PYTHON "Build the Networkz CPython extensions" OFF)

  define_option(NETWORKZ_S3 "Build Networkz with S3 support (requires the AWS SDK for C++)" OFF)

  define_option(NETWORKZ_TENSORFLOW "Build Networkz with TensorFlow support enabled" OFF)

  define_option(NETWORKZ_TESTING "Build the Networkz testing libraries" OFF)

  define_option(NETWORTKZ_BUILD_DOC "Build API documents by Oxygen" OFF)

  #----------------------------------------------------------------------
  set_option_category("Thirdparty toolchain")

  # Determine how we will look for dependencies
  # * AUTO: Guess which packaging systems we're running in and pull the
  #   dependencies from there. Build any missing ones through the
  #   ExternalProject setup. This is the default unless the CONDA_PREFIX
  #   environment variable is set, in which case the CONDA method is used
  # * BUNDLED: Build dependencies through CMake's ExternalProject facility. If
  #   you wish to build individual dependencies from source instead of using
  #   one of the other methods, pass -D$NAME_SOURCE=BUNDLED
  # * SYSTEM: Use CMake's find_package and find_library without any custom
  #   paths. If individual packages are on non-default locations, you can pass
  #   $NAME_ROOT arguments to CMake, or set environment variables for the same
  #   with CMake 3.11 and higher.  If your system packages are in a non-default
  #   location, or if you are using a non-standard toolchain, you can also pass
  #   NETWORKZ_PACKAGE_PREFIX to set the *_ROOT variables to look in that
  #   directory
  # * CONDA: Same as system but set all *_ROOT variables to
  #   ENV{CONDA_PREFIX}. If this is run within an active conda environment,
  #   then ENV{CONDA_PREFIX} will be used for dependencies unless
  #   NETWORKZ_DEPENDENCY_SOURCE is set explicitly to one of the other options
  # * BREW: Use SYSTEM but search for select packages with brew.
  if(NOT "$ENV{CONDA_PREFIX}" STREQUAL "")
    set(NETWORKZ_DEPENDENCY_SOURCE_DEFAULT "CONDA")
  else()
    set(NETWORKZ_DEPENDENCY_SOURCE_DEFAULT "AUTO")
  endif()
  define_option_string(NETWORKZ_DEPENDENCY_SOURCE
                       "Method to use for acquiring Networkz's build dependencies"
                       "${NETWORKZ_DEPENDENCY_SOURCE_DEFAULT}"
                       "AUTO"
                       "BUNDLED"
                       "SYSTEM"
                       "CONDA"
                       "BREW")

  define_option(NETWORKZ_VERBOSE_THIRDPARTY_BUILD
                "Show output from ExternalProjects rather than just logging to files" OFF)

  define_option(NETWORKZ_DEPENDENCY_USE_SHARED "Link to shared libraries" ON)

  define_option(NETWORKZ_BOOST_USE_SHARED "Rely on boost shared libraries where relevant"
                ${NETWORKZ_DEPENDENCY_USE_SHARED})

  define_option(NETWORKZ_BROTLI_USE_SHARED "Rely on Brotli shared libraries where relevant"
                ${NETWORKZ_DEPENDENCY_USE_SHARED})

  define_option(NETWORKZ_BZ2_USE_SHARED "Rely on Bz2 shared libraries where relevant"
                ${NETWORKZ_DEPENDENCY_USE_SHARED})

  define_option(NETWORKZ_GFLAGS_USE_SHARED "Rely on GFlags shared libraries where relevant"
                ${NETWORKZ_DEPENDENCY_USE_SHARED})

  define_option(NETWORKZ_GRPC_USE_SHARED "Rely on gRPC shared libraries where relevant"
                ${NETWORKZ_DEPENDENCY_USE_SHARED})

  define_option(NETWORKZ_LZ4_USE_SHARED "Rely on lz4 shared libraries where relevant"
                ${NETWORKZ_DEPENDENCY_USE_SHARED})

  define_option(NETWORKZ_OPENSSL_USE_SHARED "Rely on OpenSSL shared libraries where relevant"
                ${NETWORKZ_DEPENDENCY_USE_SHARED})

  define_option(NETWORKZ_PROTOBUF_USE_SHARED
                "Rely on Protocol Buffers shared libraries where relevant"
                ${NETWORKZ_DEPENDENCY_USE_SHARED})

  if(WIN32)
    # It seems that Thrift doesn't support DLL well yet.
    # MSYS2, conda-forge and vcpkg don't build shared library.
    set(NETWORKZ_THRIFT_USE_SHARED_DEFAULT OFF)
  else()
    set(NETWORKZ_THRIFT_USE_SHARED_DEFAULT ${NETWORKZ_DEPENDENCY_USE_SHARED})
  endif()
  define_option(NETWORKZ_THRIFT_USE_SHARED "Rely on thrift shared libraries where relevant"
                ${NETWORKZ_THRIFT_USE_SHARED_DEFAULT})

  define_option(NETWORKZ_UTF8PROC_USE_SHARED
                "Rely on utf8proc shared libraries where relevant"
                ${NETWORKZ_DEPENDENCY_USE_SHARED})

  define_option(NETWORKZ_SNAPPY_USE_SHARED "Rely on snappy shared libraries where relevant"
                ${NETWORKZ_DEPENDENCY_USE_SHARED})

  define_option(NETWORKZ_UTF8PROC_USE_SHARED
                "Rely on utf8proc shared libraries where relevant"
                ${NETWORKZ_DEPENDENCY_USE_SHARED})

  define_option(NETWORKZ_ZSTD_USE_SHARED "Rely on zstd shared libraries where relevant"
                ${NETWORKZ_DEPENDENCY_USE_SHARED})

  define_option(NETWORKZ_USE_GLOG "Build libraries with glog support for pluggable logging"
                OFF)

  define_option(NETWORKZ_WITH_BACKTRACE "Build with backtrace support" ON)

  define_option(NETWORKZ_WITH_BROTLI "Build with Brotli compression" OFF)
  define_option(NETWORKZ_WITH_BZ2 "Build with BZ2 compression" OFF)
  define_option(NETWORKZ_WITH_LZ4 "Build with lz4 compression" OFF)
  define_option(NETWORKZ_WITH_SNAPPY "Build with Snappy compression" OFF)
  define_option(NETWORKZ_WITH_ZLIB "Build with zlib compression" OFF)
  define_option(NETWORKZ_WITH_ZSTD "Build with zstd compression" OFF)

  define_option(
    NETWORKZ_WITH_UTF8PROC
    "Build with support for Unicode properties using the utf8proc library;(only used if NETWORKZ_COMPUTE is ON)"
    ON)
  define_option(
    NETWORKZ_WITH_RE2
    "Build with support for regular expressions using the re2 library;(only used if NETWORKZ_COMPUTE or NETWORKZ_GANDIVA is ON)"
    ON)

  #----------------------------------------------------------------------
  if(MSVC_TOOLCHAIN)
    set_option_category("MSVC")

    define_option(MSVC_LINK_VERBOSE
                  "Pass verbose linking options when linking libraries and executables"
                  OFF)

    define_option_string(BROTLI_MSVC_STATIC_LIB_SUFFIX
                         "Brotli static lib suffix used on Windows with MSVC" "-static")

    define_option_string(PROTOBUF_MSVC_STATIC_LIB_SUFFIX
                         "Protobuf static lib suffix used on Windows with MSVC" "")

    define_option_string(RE2_MSVC_STATIC_LIB_SUFFIX
                         "re2 static lib suffix used on Windows with MSVC" "_static")

    if(DEFINED ENV{CONDA_PREFIX})
      # Conda package changes the output name.
      # https://github.com/conda-forge/snappy-feedstock/blob/master/recipe/windows-static-lib-name.patch
      set(SNAPPY_MSVC_STATIC_LIB_SUFFIX_DEFAULT "_static")
    else()
      set(SNAPPY_MSVC_STATIC_LIB_SUFFIX_DEFAULT "")
    endif()
    define_option_string(SNAPPY_MSVC_STATIC_LIB_SUFFIX
                         "Snappy static lib suffix used on Windows with MSVC"
                         "${SNAPPY_MSVC_STATIC_LIB_SUFFIX_DEFAULT}")

    define_option_string(LZ4_MSVC_STATIC_LIB_SUFFIX
                         "Lz4 static lib suffix used on Windows with MSVC" "_static")

    define_option_string(ZSTD_MSVC_STATIC_LIB_SUFFIX
                         "ZStd static lib suffix used on Windows with MSVC" "_static")

    define_option(NETWORKZ_USE_STATIC_CRT "Build Networkz with statically linked CRT" OFF)
  endif()

  #----------------------------------------------------------------------
  set_option_category("Advanced developer")

  define_option(NETWORKZ_EXTRA_ERROR_CONTEXT
                "Compile with extra error context (line numbers, code)" OFF)

  define_option(NETWORKZ_OPTIONAL_INSTALL
                "If enabled install ONLY targets that have already been built. Please be;\
                advised that if this is enabled 'install' will fail silently on components;\
                that have not been built" OFF)

  option(NETWORKZ_BUILD_CONFIG_SUMMARY_JSON "Summarize build configuration in a JSON file"
         ON)
endif()

macro(validate_config)
  foreach(category ${NETWORKZ_OPTION_CATEGORIES})
    set(option_names ${NETWORKZ_${category}_OPTION_NAMES})

    foreach(name ${option_names})
      set(possible_values ${${name}_OPTION_POSSIBLE_VALUES})
      set(value "${${name}}")
      if(possible_values)
        if(NOT "${value}" IN_LIST possible_values)
          message(
            FATAL_ERROR "Configuration option ${name} got invalid value '${value}'. "
                        "Allowed values: ${${name}_OPTION_ENUM}.")
        endif()
      endif()
    endforeach()

  endforeach()
endmacro()

macro(config_summary_message)
  message(STATUS "---------------------------------------------------------------------")
  message(STATUS "networkz version:                                 ${NETWORKZ_VERSION}")
  message(STATUS)
  message(STATUS "Build configuration summary:")

  message(STATUS "  Generator: ${CMAKE_GENERATOR}")
  message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
  message(STATUS "  Source directory: ${CMAKE_CURRENT_SOURCE_DIR}")
  message(STATUS "  Install prefix: ${CMAKE_INSTALL_PREFIX}")
  if(${CMAKE_EXPORT_COMPILE_COMMANDS})
    message(
      STATUS "  Compile commands: ${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json")
  endif()

  foreach(category ${NETWORKZ_OPTION_CATEGORIES})

    message(STATUS)
    message(STATUS "${category} options:")
    message(STATUS)

    set(option_names ${NETWORKZ_${category}_OPTION_NAMES})

    foreach(name ${option_names})
      set(value "${${name}}")
      if("${value}" STREQUAL "")
        set(value "\"\"")
      endif()

      set(description ${${name}_OPTION_DESCRIPTION})

      if(NOT ("${${name}_OPTION_ENUM}" STREQUAL ""))
        set(summary "=${value} [default=${${name}_OPTION_ENUM}]")
      else()
        set(summary "=${value} [default=${${name}_OPTION_DEFAULT}]")
      endif()

      message(STATUS "  ${name}${summary}")
      foreach(description_line ${description})
        message(STATUS "      ${description_line}")
      endforeach()
    endforeach()

  endforeach()

endmacro()


macro(config_summary_json)
  set(summary "${CMAKE_CURRENT_BINARY_DIR}/cmake_summary.json")
  message(STATUS "  Outputting build configuration summary to ${summary}")
  file(WRITE ${summary} "{\n")

  foreach(category ${NETWORKZ_OPTION_CATEGORIES})
    foreach(name ${NETWORKZ_${category}_OPTION_NAMES})
      file(APPEND ${summary} "\"${name}\": \"${${name}}\",\n")
    endforeach()
  endforeach()

  file(APPEND ${summary} "\"generator\": \"${CMAKE_GENERATOR}\",\n")
  file(APPEND ${summary} "\"build_type\": \"${CMAKE_BUILD_TYPE}\",\n")
  file(APPEND ${summary} "\"source_dir\": \"${CMAKE_CURRENT_SOURCE_DIR}\",\n")
  if(${CMAKE_EXPORT_COMPILE_COMMANDS})
    file(APPEND ${summary} "\"compile_commands\": "
                           "\"${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json\",\n")
  endif()
  file(APPEND ${summary} "\"install_prefix\": \"${CMAKE_INSTALL_PREFIX}\",\n")
  file(APPEND ${summary} "\"networkz_version\": \"${NETWORKZ_VERSION}\"\n")
  file(APPEND ${summary} "}\n")
endmacro()