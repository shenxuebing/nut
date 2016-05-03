﻿/**
 * see http://sourceforge.net/p/predef/wiki/Home/
 */

#ifndef ___HEADFILE___6689E00A_B262_4C15_A826_19665AF8EC32_
#define ___HEADFILE___6689E00A_B262_4C15_A826_19665AF8EC32_


/**
 * 操作系统
 *
 * 支持检测：
 *  NUT_PLATFORM_OS_WINDOWS
 *  NUT_PLATFORM_OS_MAC
 *  NUT_PLATFORM_OS_LINUX
 */
#if defined(__WINDOWS__) || defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64) || \
    defined(__WIN32__) || defined(__TOS_WIN__)
#   define NUT_PLATFORM_OS_WINDOWS 1
#   define NUT_PLATFORM_OS_MAC     0
#   define NUT_PLATFORM_OS_LINUX   0
#elif defined(__APPLE__) || defined(__apple__) || defined(__MACOSX) || defined(__MACOS_CLASSIC__)
#   define NUT_PLATFORM_OS_WINDOWS 0
#   define NUT_PLATFORM_OS_MAC     1
#   define NUT_PLATFORM_OS_LINUX   0
#elif defined(__linux__) || defined(linux) || defined(__linux) || defined(__LINUX__) || defined(LINUX) || \
    defined(_LINUX)
#   define NUT_PLATFORM_OS_WINDOWS 0
#   define NUT_PLATFORM_OS_MAC     0
#   define NUT_PLATFORM_OS_LINUX   1
#else
#   error unknown OS
#endif

/**
 * 系统位数
 *
 * 支持检测：
 *  NUT_PLATFORM_BITS_32
 *  NUT_PLATFORM_BITS_64
 */
#if defined(_WIN64) || defined(WIN64) || defined(__amd64__) || defined(__amd64) || defined(__LP64__) || \
    defined(_LP64) || defined(__x86_64__) || defined(__x86_64) || defined(_M_X64) || defined(__ia64__) || \
    defined(_IA64) || defined(__IA64__) || defined(__ia64) || defined(_M_IA64)
#   define NUT_PLATFORM_BITS_16 0
#   define NUT_PLATFORM_BITS_32 0
#   define NUT_PLATFORM_BITS_64 1
#elif defined(_WIN32) || defined(WIN32) || defined(__32BIT__) || defined(__ILP32__) || defined(_ILP32) || \
    defined(i386) || defined(__i386__) || defined(__i486__) || defined(__i586__) || defined(__i686__) || \
    defined(__i386) || defined(_M_IX86) || defined(__X86__) || defined(_X86_) || defined(__I86__)
#   define NUT_PLATFORM_BITS_16 0
#   define NUT_PLATFORM_BITS_32 1
#   define NUT_PLATFORM_BITS_64 0
#else
#   error unknown system bit-length
#endif

/**
 * 编译器
 *
 * 支持检测：
 *  NUT_PLATFORM_CC_VC
 *  NUT_PLATFORM_CC_MINGW
 *  NUT_PLATFORM_CC_GCC
 */
#if defined(_MSC_VER)
#	define NUT_PLATFORM_CC_VC    1
#	define NUT_PLATFORM_CC_MINGW 0
#	define NUT_PLATFORM_CC_GCC   0
#elif defined(__MINGW32__) || defined(__MINGW64__)
#	define NUT_PLATFORM_CC_VC    0
#	define NUT_PLATFORM_CC_MINGW 1
#	define NUT_PLATFORM_CC_GCC   1
#elif defined(__GNUG__) || defined(__GNUC__)
#	define NUT_PLATFORM_CC_VC    0
#	define NUT_PLATFORM_CC_MINGW 0
#	define NUT_PLATFORM_CC_GCC   1
#else
#   error unknown compiler
#endif

/** 模块API定义工具 */
#if NUT_PLATFORM_OS_WINDOWS
#   define DLL_API extern "C" __declspec(dllexport)
#else
#   define DLL_API extern "C"
#endif

/** 用于消除编译时 unused parameter 警告 */
#ifndef UNUSED
#   define UNUSED(x) ((void)x)
#endif

#endif /* head file guarder */
