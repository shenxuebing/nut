# NUT 库 CMakeLists.txt

# 收集所有源文件
file(GLOB_RECURSE NUT_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
)

# 收集所有头文件
file(GLOB_RECURSE NUT_HEADERS 
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

# 打印找到的源文件数量
list(LENGTH NUT_SOURCES SOURCE_COUNT)
list(LENGTH NUT_HEADERS HEADER_COUNT)
message(STATUS "Found ${SOURCE_COUNT} source files and ${HEADER_COUNT} header files for nut library")

# 根据配置创建库（动态库或静态库）
if(WIN32)
    add_library(nut ${NUT_LIBRARY_TYPE} ${NUT_SOURCES} ${NUT_HEADERS})
else()
    # 非Windows平台默认创建动态库
    add_library(nut SHARED ${NUT_SOURCES} ${NUT_HEADERS})
endif()

# 如果在 Windows 且构建为静态库，给静态库文件名添加后缀以避免与动态库冲突
if(WIN32 AND NUT_LIBRARY_TYPE STREQUAL "STATIC")
    get_property(_suf GLOBAL PROPERTY NUT_STATIC_LIB_SUFFIX)
    if(_suf)
        set_target_properties(nut PROPERTIES ARCHIVE_OUTPUT_NAME "nut${_suf}")
    endif()
endif()

# 设置库属性
set_target_properties(nut PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    OUTPUT_NAME "nut"
    POSITION_INDEPENDENT_CODE ON
)

# Windows特定设置
if(WIN32 AND MSVC)
    if(NUT_LIBRARY_TYPE STREQUAL "STATIC")
        # 静态库特定设置
        target_compile_definitions(nut PUBLIC NUT_STATIC_LIB)
        set_target_properties(nut PROPERTIES
            OUTPUT_NAME "nut_static"
        )
        message(STATUS "Building nut as static library")
    else()
        # 动态库特定设置
        target_compile_definitions(nut PRIVATE BUILDING_NUT_DLL)
        target_compile_definitions(nut PUBLIC NUT_SHARED_LIB)
        message(STATUS "Building nut as dynamic library")
    endif()
    
    # 设置运行时库
    set_property(TARGET nut PROPERTY
        MSVC_RUNTIME_LIBRARY ${CMAKE_MSVC_RUNTIME_LIBRARY}
    )
endif()

# 包含目录
target_include_directories(nut 
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# 编译定义
target_compile_definitions(nut 
    PRIVATE 
        BUILDING_NUT
)

# 平台特定的链接库
if(PLATFORM_LINUX)
    target_link_libraries(nut 
        PUBLIC 
            Threads::Threads
    )
    
    # 查找并链接原子操作库和dl库
    if(ATOMIC_LIBRARY)
        target_link_libraries(nut PUBLIC ${ATOMIC_LIBRARY})
    endif()
    
    if(DL_LIBRARY)
        target_link_libraries(nut PUBLIC ${DL_LIBRARY})
    endif()
    
elseif(PLATFORM_DARWIN)
    # macOS 特定设置
    target_link_libraries(nut PUBLIC "-framework Foundation")
    
endif()

# C++标准库链接
if(NOT PLATFORM_DARWIN AND NOT PLATFORM_WINDOWS)
    target_link_libraries(nut PUBLIC stdc++)
elseif(PLATFORM_DARWIN)
    target_link_libraries(nut PUBLIC c++)
endif()

# 安装库文件
install(TARGETS nut
    EXPORT nutTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# 为开发构建创建符号链接（仅Unix系统）
if(UNIX AND NOT APPLE)
    add_custom_command(TARGET nut POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E create_symlink
        $<TARGET_FILE:nut>
        ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libnut.so
        COMMENT "Creating symlink for nut library"
    )
endif()

# 为调试版本添加调试信息
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        target_compile_options(nut PRIVATE -g3 -ggdb)
    endif()
endif()

# 编译警告设置
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(nut PRIVATE
        -Wall -Wextra -Wpedantic
        -Wno-unused-parameter
        -Wno-missing-field-initializers
    )
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(nut PRIVATE
        -Wall -Wextra -Wpedantic
        -Wno-unused-parameter
        -Wno-missing-field-initializers
        -Wno-gnu-zero-variadic-macro-arguments
    )
elseif(MSVC)
    target_compile_options(nut PRIVATE
        /W3
        /wd4996  # 禁用已弃用函数警告
        /wd4267  # 禁用size_t转换警告
        /wd4244  # 禁用类型转换警告
    )
endif()

# 输出库信息
message(STATUS "NUT Library Configuration:")
message(STATUS "  Library Type: SHARED")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  Sources: ${SOURCE_COUNT} files")
message(STATUS "  Headers: ${HEADER_COUNT} files")
message(STATUS "  Install Path: ${CMAKE_INSTALL_LIBDIR}")

