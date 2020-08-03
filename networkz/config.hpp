//
//  config.hpp
//  networkz
//
//  Created by 曹巍 on 2020/07/31.
//  Copyright © 2020 曹巍. All rights reserved.
//

#ifndef config_h
#define config_h

#if defined(_MSC_VER)
/* MSVC 2015+ can use a clang frontend, so we want to label it only as MSVC
 * and not MSVC and clang. */
#define NX_COMPILER_MSVC

#elif defined(__INTEL_COMPILER)
/* Intel 14+ on OSX uses a clang frontend, so again we want to label them as
 * intel only, and not intel and clang. */
#define NX_COMPILER_ICC

#elif defined(__PGI)
/* PGI reports as GNUC as it generates the same ABI, so we need to check for
 * it before gcc. */
#define NX_COMPILER_PGI

#elif defined(__clang__)
/* Check for clang before GCC, as clang says it is GNUC since it has ABI
 * compliance and supports many of the same extensions. */
#define NX_COMPILER_CLANG

#elif defined(__GNUC__)
/* Several compilers pretend to be GCC but have minor differences. To
 * compensate for that, we checked for those compilers first above. */
#define NX_COMPILER_GCC
#define NX_COMPILER_GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + \
__GNUC_PATCHLEVEL__)
#endif

/*--------------------------------------------------------------------------*/
/* Platform Features                                                        */

/* Byte order.  */
/* All compilers that support Mac OS X define either __BIG_ENDIAN__ or
 __LITTLE_ENDIAN__ to match the endianness of the architecture being
 compiled for. This is not necessarily the same as the architecture of the
 machine doing the building. In order to support Universal Binaries on
 Mac OS X, we prefer those defines to decide the endianness.
 On other platforms we use the result of the TRY_RUN. */
#if !defined(__APPLE__)
/* #undef NX_WORDS_BIGENDIAN */
#elif defined(__BIG_ENDIAN__)
#define NX_WORDS_BIGENDIAN
#endif

/* Threading system.  */
/* #undef NX_USE_PTHREADS */
/* #undef NX_USE_SPROC */
/* #undef NX_HP_PTHREADS */
//#define NX_USE_WIN32_THREADS
#define NX_USE_PTHREADS
#define NX_MAX_THREADS 64

/* Atomic operations */
/* #undef NX_HAVE_SYNC_BUILTINS */
#if defined(_WIN32)
#define NX_HAS_INTERLOCKEDADD
#endif

/* NXSMPTools back-end */
#define NX_SMP_Sequential
#define NX_SMP_BACKEND "Sequential"

/* Compiler features.  */
/* #undef NX_HAVE_GETSOCKNAME_WITH_SOCKLEN_T */
/* #undef NX_HAVE_SO_REUSEADDR */

/* Whether we require large files support.  */
/* #undef NX_REQUIRE_LARGE_FILE_SUPPORT */

/* Whether reverse const iterator's have comparison operators. */
#define NX_CONST_REVERSE_ITERATOR_COMPARISON

/*--------------------------------------------------------------------------*/
/* NX Platform Configuration                                               */

/* Whether the target platform supports shared libraries.  */
/* #undef NX_TARGET_SUPPORTS_SHARED_LIBS */

/* Whether we are building shared libraries.  */
/* #undef NX_BUILD_SHARED_LIBS */

/* Whether NXIdType is a 64-bit integer type (or a 32-bit integer type).  */
#define NX_USE_64BIT_IDS

/* Whether MTime should use a 64-bit integer type on 32 bit builds.  */
/* #undef NX_USE_64BIT_TIMESTAMPS */

/* C++ compiler used.  */
/* #define NX_CXX_COMPILER "C:/Program Files (x86)/Microsoft Visual Studio/2017/Enterprise/VC/Tools/MSVC/14.10.25017/bin/HostX86/x64/cl.exe"
*/
#define NX_CXX_COMPILER "/usr/bin/clang++"

/* Compatibility settings.  */
/* #undef NX_LEGACY_REMOVE */
/* #undef NX_LEGACY_SILENT */

/* Debug leaks support.  */
/* #undef NX_DEBUG_LEAKS */

/* Print warning on NXArrayDispatch failure.  */
/* #undef NX_WARN_ON_DISPATCH_FAILURE */

/* Should all New methods use the object factory override. */
/* #undef NX_ALL_NEW_OBJECT_FACTORY */

/*--------------------------------------------------------------------------*/
/* Setup NX based on platform features and configuration.                  */

/* We now always use standard streams.  */
#ifndef NX_LEGACY_REMOVE
# define NX_USE_ANSI_STDLIB
#endif

/* Define a "NXstd_bool" for backwards compatibility.  Only use bool
 if this file is included by a c++ file. */
#ifndef NX_LEGACY_REMOVE
# if defined(__cplusplus)
typedef bool NX_STD_BOOL;
# else
typedef int NX_STD_BOOL;
# endif
#endif

/* The maximum length of a file name in bytes including the
 * terminating null.
 */
#if defined(PATH_MAX) // Usually defined on Windows
# define NX_MAXPATH PATH_MAX
#elif defined(MAXPATHLEN)  // Usually defined on linux
# define NX_MAXPATH MAXPATHLEN
#else
# define NX_MAXPATH 32767 // Possible with Windows "extended paths"
#endif

#if (__cplusplus >= 201103L) || ( defined(_MSC_VER) && _MSC_VER >= 1700 )
# define NX_OVERRIDE override
# define NX_FINAL final
#else
# define NX_OVERRIDE
# define NX_FINAL
#endif

#if (__cplusplus >= 201103L) || ( defined(_MSC_VER) && _MSC_VER >= 1800 )
# define NX_DELETE_FUNCTION =delete
#else
# define NX_DELETE_FUNCTION
#endif

/** extern template declarations for C++11
 */
#if ((__cplusplus >= 201103L) || (defined(_MSC_VER) && _MSC_VER >= 1700)) && !defined(__MINGW32__)
#define NX_USE_EXTERN_TEMPLATE
#endif

/** std::auto_ptr is deprecated in C++11 and will be removed in C++17
 *  use std::unique_ptr in C++11 and later compilers.
 */
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >=1700)\
|| (defined(_CPPLIB_VER) && _CPPLIB_VER >= 520)
# define NX_HAS_STD_UNIQUE_PTR
#endif

/* MS Visual Studio 2015 finally supports C99/C++11's snprintf but
 * for older versions, use _snprintf instead. Annoyingly, its semantics
 * are slightly different, but still better than using sprintf.
 */
#if defined(_MSC_VER) && (_MSC_VER < 1900)
# define snprintf _snprintf
#endif

#endif /* config_h */
