//
//  sysutil.hpp
//  networkz
//
//  Created by 曹巍 on 2020/12/12.
//  Copyright © 2020 曹巍. All rights reserved.
//
// This code is belong to a part of RAxMML-NG project which is licensed under
// the GNU Affero General Public License version 3.
//
// Reference:
//
// Alexey M. Kozlov, Diego Darriba, Tomáš Flouri, Benoit Morel,
// and Alexandros Stamatakis (2019) RAxML-NG: A fast, scalable, and
// user-friendly tool for maximum likelihood phylogenetic inference.
// Bioinformatics, btz305 doi:10.1093/bioinformatics/btz305
#ifndef sysutil_hpp
#define sysutil_hpp

#include <unistd.h>

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdexcept>

#include <string>
#include <string.h>
#include <array>
#include <fstream>

#if (!defined(__APPLE__) && !defined(__WIN32__) && !defined(__WIN64__))
#include <sys/sysinfo.h>
#endif

//#if (defined(__WIN32__) || defined(__WIN64__))
//#define NZ_EXPORT __declspec(dllexport)
//#else
//#define NZ_EXPORT
//#endif
//
//#define NZ_MIN(a,b) ((a) < (b) ? (a) : (b))
//#define NZ_MAX(a,b) ((a) > (b) ? (a) : (b))
//#define NZ_SWAP(x,y) do { __typeof__ (x) _t = x; x = y; y = _t; } while(0)
//#define NZ_STAT(x) ((pll_hardware.init || nz_hardware_probe()) \
//                     && pll_hardware.x)
//
//// Attribute flags
//#define NZ_ATTRIB_ARCH_CPU            0
//#define NZ_ATTRIB_ARCH_SSE       (1 << 0)
//#define NZ_ATTRIB_ARCH_AVX       (1 << 1)
//#define NZ_ATTRIB_ARCH_AVX2      (1 << 2)
//#define NZ_ATTRIB_ARCH_AVX512    (1 << 3)
//#define NZ_ATTRIB_ARCH_MASK         0xF
//
//// used to suppress compiler warnings about unused args
#define NZ_UNUSED(expr) (void)(expr)
//
// cpu features
#define NZ_CPU_SSE3  (1<<0)
#define NZ_CPU_AVX   (1<<1)
#define NZ_CPU_FMA3  (1<<2)
#define NZ_CPU_AVX2  (1<<3)

//
//typedef struct _hardware_s
//{
//  int init;
//  /* cpu features */
//  int altivec_present;
//  int mmx_present;
//  int sse_present;
//  int sse2_present;
//  int sse3_present;
//  int ssse3_present;
//  int sse41_present;
//  int sse42_present;
//  int popcnt_present;
//  int avx_present;
//  int avx2_present;
//
//} nz_hardware_type;


void sysutil_fatal(const char * format, ...);

double sysutil_gettime();
void sysutil_show_rusage();
unsigned long sysutil_get_memused();
unsigned long sysutil_get_memtotal(bool ignore_errors = true);

std::string sysutil_get_cpu_model();
unsigned int sysutil_get_cpu_cores();
unsigned long sysutil_get_cpu_features();
//unsigned int sysutil_simd_autodetect();

double sysutil_get_energy();

std::string sysutil_fmt_time(const time_t& t);

std::string sysutil_realpath(const std::string& path);
bool sysutil_file_exists(const std::string& fname, int access_mode = F_OK);
bool sysutil_dir_exists(const std::string& dname);
void sysutil_file_remove(const std::string& fname, bool must_exist);

bool sysutil_isnumber(const std::string& str);

//#ifdef __cplusplus
//extern "C" {
//#endif
//NZ_EXPORT int nz_hardware_probe(void);
//#ifdef __cplusplus
//} /* extern "C" */
//#endif
#endif /* sysutil_hpp */
