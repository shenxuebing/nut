cmake_minimum_required(VERSION 3.10)

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

# 设置构建类型
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# 设置运行时库类型 (仅Windows)
if(WIN32)
    # 保留 NUT_RUNTIME_LIBRARY 以控制 MSVC 运行时，但不再由它决定输出路径
    set(NUT_RUNTIME_LIBRARY "MD" CACHE STRING "Runtime library type (MT/MD)")
    set_property(CACHE NUT_RUNTIME_LIBRARY PROPERTY STRINGS "MT" "MD")

    # 原来的 NUT_LIBRARY_TYPE 仍可控制库的构建类型（子项目可能使用），但输出路径不再按 shared/static 分裂
    set(NUT_LIBRARY_TYPE "SHARED" CACHE STRING "Library type (SHARED/STATIC)")
    set_property(CACHE NUT_LIBRARY_TYPE PROPERTY STRINGS "SHARED" "STATIC")

    # 新增：通过 BUILD_RUNTIME 选择输出目录顶层（MD 或 MT），仅在 Windows 有效
    if(NOT DEFINED BUILD_RUNTIME)
      set(BUILD_RUNTIME "MD" CACHE STRING "Choose runtime dir for outputs: MD or MT (Windows only)")
    endif()
    string(TOUPPER "${BUILD_RUNTIME}" BUILD_RUNTIME)
    if(NOT BUILD_RUNTIME STREQUAL "MD" AND NOT BUILD_RUNTIME STREQUAL "MT")
      message(FATAL_ERROR "BUILD_RUNTIME must be either MD or MT (case-insensitive).")
    endif()

    set(OUT_DIR_BIN "${CMAKE_BINARY_DIR}/${BUILD_RUNTIME}/bin")
    set(OUT_DIR_LIB "${CMAKE_BINARY_DIR}/${BUILD_RUNTIME}/lib")

    # Ensure directories exist at configure time
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
    # 这通过设置目标属性实现；子目录/目标如果显式设置了 OUTPUT_NAME/ARCHIVE_OUTPUT_NAME 则可能需调整。
    set_property(GLOBAL PROPERTY NUT_STATIC_LIB_SUFFIX "_static")

    # 为兼容旧逻辑，仍按 NUT_RUNTIME_LIBRARY 设置 MSVC 运行时
    if(MSVC)
      if(NUT_RUNTIME_LIBRARY STREQUAL "MT")
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
        message(STATUS "Using static runtime library (MT/MTd)")
      else()
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
        message(STATUS "Using dynamic runtime library (MD/MDd)")
      endif()
    endif()

else()
    # 非Windows平台的默认输出目录（保留原行为）
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
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
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
    elseif(PLATFORM_LINUX)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -rdynamic")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -rdynamic")
    endif()
elseif(MSVC)
    # MSVC 设置
    add_definitions(-DBUILDING_NUT -D_ENABLE_ATOMIC_ALIGNMENT_FIX)
    
    # 添加UTF-8编码支持
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /utf-8")
    
    # 运行时库设置
    if(NUT_RUNTIME_LIBRARY STREQUAL "MT")
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
        message(STATUS "Using static runtime library (MT/MTd)")
    else()
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
        message(STATUS "Using dynamic runtime library (MD/MDd)")
    endif()
    
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_definitions(-DNDEBUG)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O2")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /O2")
    else()
        add_definitions(-DDEBUG)
    endif()
    
    # 禁用安全警告和其他警告
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4996 /wd4251 /wd4275")
    
    # 多处理器编译
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP")
endif()

# 添加构建定义
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
    message(STATUS "  Library Type: ${NUT_LIBRARY_TYPE}")
    message(STATUS "  Runtime Library: ${NUT_RUNTIME_LIBRARY}")
    message(STATUS "  Output Directory: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
endif()
message(STATUS "  Install Prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "  Library Directory: ${CMAKE_INSTALL_LIBDIR}")
message(STATUS "  Include Directory: ${CMAKE_INSTALL_INCLUDEDIR}")

