cmake_minimum_required(VERSION 3.15)

# ============ CRT 配置必须在 project() 之前 ============
if(WIN32)
    # 确保 CMAKE_MSVC_RUNTIME_LIBRARY 策略生效
    if(POLICY CMP0091)
        cmake_policy(SET CMP0091 NEW)
    endif()
    
    # 向后兼容：处理旧的变量名
    if(DEFINED NUT_RUNTIME_LIBRARY)
        if(NUT_RUNTIME_LIBRARY STREQUAL "MT")
            set(STATIC_CRT ON CACHE BOOL "Build with static CRT (/MT)" FORCE)
            message(STATUS "Detected NUT_RUNTIME_LIBRARY=MT, setting STATIC_CRT=ON")
        else()
            set(STATIC_CRT OFF CACHE BOOL "Build with static CRT (/MT)" FORCE)
            message(STATUS "Detected NUT_RUNTIME_LIBRARY=MD, setting STATIC_CRT=OFF")
        endif()
    endif()
    
    if(DEFINED BUILD_RUNTIME)
        if(BUILD_RUNTIME STREQUAL "MT")
            set(STATIC_CRT ON CACHE BOOL "Build with static CRT (/MT)" FORCE)
            message(STATUS "Detected BUILD_RUNTIME=MT, setting STATIC_CRT=ON")
        else()
            set(STATIC_CRT OFF CACHE BOOL "Build with static CRT (/MT)" FORCE)
            message(STATUS "Detected BUILD_RUNTIME=MD, setting STATIC_CRT=OFF")
        endif()
    endif()
    
    # 使用统一的 STATIC_CRT 选项
    option(STATIC_CRT "Build with static CRT (/MT)" OFF)
    
    # 设置 CMAKE_MSVC_RUNTIME_LIBRARY（必须在 project() 之前）
    if(STATIC_CRT)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
        message(STATUS "NUT: Using static runtime library (/MT for Release, /MTd for Debug)")
    else()
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL$<$<CONFIG:Debug>:Debug>")
        message(STATUS "NUT: Using dynamic runtime library (/MD for Release, /MDd for Debug)")
    endif()
endif()

# 项目信息
project(nut
    VERSION 1.0.0
    DESCRIPTION "Nut C++ Utility Library"
    LANGUAGES CXX C
)

# 设置C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

option(NUT_STATIC_LIBSTDCXX "Link libstdc++ statically when the static archive is available" ON)

# 设置构建类型
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# 处理 NUT_LIBRARY_TYPE 变量（所有平台通用）
if(DEFINED NUT_LIBRARY_TYPE)
    if(NUT_LIBRARY_TYPE STREQUAL "STATIC")
        set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)
        set(BUILD_STATIC_LIBS ON CACHE BOOL "Build static libraries" FORCE)
        message(STATUS "NUT_LIBRARY_TYPE=STATIC: Building static library only")
    elseif(NUT_LIBRARY_TYPE STREQUAL "SHARED")
        set(BUILD_SHARED_LIBS ON CACHE BOOL "Build shared libraries" FORCE)
        set(BUILD_STATIC_LIBS OFF CACHE BOOL "Build static libraries" FORCE)
        message(STATUS "NUT_LIBRARY_TYPE=SHARED: Building shared library only")
    else()
        message(WARNING "NUT_LIBRARY_TYPE=${NUT_LIBRARY_TYPE} is not valid. Use STATIC or SHARED.")
    endif()
endif()

# ============ Windows 平台其他配置 ============
if(WIN32)
    
    # 库类型选项
    option(BUILD_SHARED_LIBS "Build shared libraries" ON)
    option(BUILD_STATIC_LIBS "Build static libraries" OFF)
    
    # 确保至少构建一种库
    if(NOT BUILD_SHARED_LIBS AND NOT BUILD_STATIC_LIBS)
        set(BUILD_STATIC_LIBS ON)
    endif()
    
    # 输出目录配置
    set(OUT_DIR_BIN "${CMAKE_BINARY_DIR}/bin")
    set(OUT_DIR_LIB "${CMAKE_BINARY_DIR}/lib")
    
    file(MAKE_DIRECTORY "${OUT_DIR_BIN}")
    file(MAKE_DIRECTORY "${OUT_DIR_LIB}")

    # 全局输出目录（默认及 multi-config）
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${OUT_DIR_BIN}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${OUT_DIR_LIB}")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${OUT_DIR_LIB}")
    foreach(CONFIG_TYPE IN ITEMS Debug Release RelWithDebInfo MinSizeRel)
      set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG_TYPE} "${OUT_DIR_BIN}")
      set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONFIG_TYPE} "${OUT_DIR_LIB}")
      set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONFIG_TYPE} "${OUT_DIR_LIB}")
    endforeach()

    # 为避免静态库文件名与动态库冲突：在 Windows 下给静态库添加后缀 "_static"
    set_property(GLOBAL PROPERTY NUT_STATIC_LIB_SUFFIX "_static")

    # MSVC 设置
    add_definitions(-DBUILDING_NUT -D_ENABLE_ATOMIC_ALIGNMENT_FIX)
    
    # 添加UTF-8编码支持
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /utf-8")
    
    # 禁用安全警告和其他警告
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4996 /wd4251 /wd4275")
    
    # 多处理器编译
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP")
else()
    # 非Windows平台的默认输出目录（保留原行为）
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
    
    # 非Windows平台的库类型选项
    option(BUILD_SHARED_LIBS "Build shared libraries" ON)
    option(BUILD_STATIC_LIBS "Build static libraries" OFF)
    
    # 确保至少构建一种库
    if(NOT BUILD_SHARED_LIBS AND NOT BUILD_STATIC_LIBS)
        set(BUILD_STATIC_LIBS ON)
    endif()
endif()

# 平台检测
if(WIN32)
    set(PLATFORM_WINDOWS TRUE)
    set(PLATFORM_NAME "Windows")
elseif(APPLE)
    set(PLATFORM_DARWIN TRUE)
    set(PLATFORM_NAME "Darwin")
elseif(UNIX)
    set(PLATFORM_LINUX TRUE)
    set(PLATFORM_NAME "Linux")
endif()

message(STATUS "Target platform: ${PLATFORM_NAME}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# 编译器特定设置
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # GCC 和 Clang 通用设置
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
    
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_definitions(-DDEBUG)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
    else()
        add_definitions(-DNDEBUG)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2")
    endif()
    
    # 平台特定设置
    if(PLATFORM_DARWIN)
        add_compile_options(-stdlib=libc++)
    elseif(PLATFORM_LINUX)
        add_compile_options(-rdynamic)
        add_link_options(-rdynamic)
        string(REPLACE "-static-libstdc++" "" CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS}")

        if(NUT_STATIC_LIBSTDCXX AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            execute_process(
                COMMAND ${CMAKE_CXX_COMPILER} -print-file-name=libstdc++.a
                OUTPUT_VARIABLE NUT_LIBSTDCXX_STATIC
                OUTPUT_STRIP_TRAILING_WHITESPACE
            )
            if(NUT_LIBSTDCXX_STATIC AND EXISTS "${NUT_LIBSTDCXX_STATIC}")
                set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -static-libstdc++")
                message(STATUS "NUT: static libstdc++ enabled: ${NUT_LIBSTDCXX_STATIC}")
            else()
                message(STATUS "NUT: static libstdc++ not found; using compiler default libstdc++ linkage")
            endif()
        endif()
    endif()
endif()

# 添加构建定义
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DDEBUG)
else()
    add_definitions(-DNDEBUG)
endif()

if(NOT MSVC)
    add_definitions(-DBUILDING_NUT)
endif()

# 查找依赖
if(PLATFORM_LINUX)
    find_package(Threads REQUIRED)
    find_library(DL_LIBRARY dl)
    
    # 查找 atomic 库，某些系统需要显式链接
    find_library(ATOMIC_LIBRARY atomic)
    if(NOT ATOMIC_LIBRARY)
        # 如果找不到 atomic 库，尝试检查是否需要链接
        include(CheckCXXSourceCompiles)
        set(CMAKE_REQUIRED_FLAGS "-std=c++11")
        check_cxx_source_compiles("
            #include <atomic>
            int main() {
                std::atomic<__int128> x{0};
                return x.compare_exchange_strong(x, x);
            }" ATOMIC_BUILTIN)
        
        if(NOT ATOMIC_BUILTIN)
            # 需要链接 atomic 库
            set(ATOMIC_LIBRARY "atomic")
            message(STATUS "Will link with atomic library for 128-bit atomic operations")
        endif()
    endif()
endif()

# 包含目录
include_directories(src)

# 添加子目录
add_subdirectory(src/nut)
add_subdirectory(src/test_nut)

# 安装配置
include(GNUInstallDirs)

# 安装头文件
install(DIRECTORY src/nut/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/nut
    FILES_MATCHING PATTERN "*.h"
)

# 生成并安装配置文件
include(CMakePackageConfigHelpers)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/nutConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/nutConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/nut
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/nutConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/nutConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/nutConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/nut
)

# 导出目标
install(EXPORT nutTargets
    FILE nutTargets.cmake
    NAMESPACE nut::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/nut
)

# 打印构建信息
message(STATUS "NUT Library Configuration Summary:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  Platform: ${PLATFORM_NAME}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
if(WIN32)
    if(BUILD_SHARED_LIBS)
        message(STATUS "  Library Type: Shared")
    endif()
    if(BUILD_STATIC_LIBS)
        message(STATUS "  Library Type: Static")
    endif()
    if(STATIC_CRT)
        message(STATUS "  Runtime Library: Static CRT (/MT)")
    else()
        message(STATUS "  Runtime Library: Dynamic CRT (/MD)")
    endif()
    message(STATUS "  Output Directory: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
endif()
message(STATUS "  Install Prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "  Library Directory: ${CMAKE_INSTALL_LIBDIR}")
message(STATUS "  Include Directory: ${CMAKE_INSTALL_INCLUDEDIR}")
