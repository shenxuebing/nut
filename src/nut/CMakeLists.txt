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

# 根据配置创建库（支持同时构建动态库和静态库）
if(BUILD_SHARED_LIBS)
    add_library(nut_shared SHARED ${NUT_SOURCES} ${NUT_HEADERS})
    add_library(nut::nut_shared ALIAS nut_shared)
    
    set_target_properties(nut_shared PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
        OUTPUT_NAME "nut"
        POSITION_INDEPENDENT_CODE ON
    )
    
    target_compile_definitions(nut_shared PRIVATE BUILDING_NUT_DLL)
    target_compile_definitions(nut_shared PUBLIC NUT_SHARED_LIB)
    
    message(STATUS "Building nut as shared library")
endif()

if(BUILD_STATIC_LIBS)
    add_library(nut_static STATIC ${NUT_SOURCES} ${NUT_HEADERS})
    add_library(nut::nut_static ALIAS nut_static)
    
    set_target_properties(nut_static PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
        OUTPUT_NAME "nut_static"
        POSITION_INDEPENDENT_CODE ON
    )
    
    target_compile_definitions(nut_static PUBLIC USE_STATIC_NUT_LIB NUT_STATIC_LIB)
    
    message(STATUS "Building nut as static library")
endif()

# 创建通用别名指向首选库类型
if(BUILD_SHARED_LIBS)
    add_library(nut ALIAS nut_shared)
else()
    add_library(nut ALIAS nut_static)
endif()

# CRT 配置由全局 CMAKE_MSVC_RUNTIME_LIBRARY 控制，不再单独设置

# 为所有库目标配置包含目录和编译定义的辅助函数
function(configure_nut_target target_name)
    target_include_directories(${target_name}
        PUBLIC 
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}
    )
    
    target_compile_definitions(${target_name}
        PRIVATE 
            BUILDING_NUT
    )
endfunction()

# 应用配置到所有库目标
if(BUILD_SHARED_LIBS)
    configure_nut_target(nut_shared)
endif()
if(BUILD_STATIC_LIBS)
    configure_nut_target(nut_static)
endif()

# 平台特定的链接库配置函数
function(configure_nut_linking target_name)
    if(PLATFORM_LINUX)
        target_link_libraries(${target_name}
            PUBLIC 
                Threads::Threads
        )
        
        if(ATOMIC_LIBRARY)
            target_link_libraries(${target_name} PUBLIC ${ATOMIC_LIBRARY})
        endif()
        
        if(DL_LIBRARY)
            target_link_libraries(${target_name} PUBLIC ${DL_LIBRARY})
        endif()
    elseif(PLATFORM_DARWIN)
        target_link_libraries(${target_name} PUBLIC "-framework Foundation")
        target_link_libraries(${target_name} PUBLIC c++)
    endif()
endfunction()

# 应用链接配置到所有库目标
if(BUILD_SHARED_LIBS)
    configure_nut_linking(nut_shared)
endif()
if(BUILD_STATIC_LIBS)
    configure_nut_linking(nut_static)
endif()

# 安装库文件
if(BUILD_SHARED_LIBS)
    install(TARGETS nut_shared
        EXPORT nutTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )
endif()

if(BUILD_STATIC_LIBS)
    install(TARGETS nut_static
        EXPORT nutTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )
endif()

# 为开发构建创建符号链接（仅Unix系统的动态库）
if(UNIX AND NOT APPLE AND BUILD_SHARED_LIBS)
    add_custom_command(TARGET nut_shared POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E create_symlink
        $<TARGET_FILE:nut_shared>
        ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libnut.so
        COMMENT "Creating symlink for nut library"
    )
endif()

# 为调试版本添加调试信息
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        if(BUILD_SHARED_LIBS)
            target_compile_options(nut_shared PRIVATE -g3 -ggdb)
        else()
            target_compile_options(nut_static PRIVATE -g3 -ggdb)
        endif()
    endif()
endif()

# 编译警告设置函数
function(configure_nut_warnings target_name)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(${target_name} PRIVATE
            -Wall -Wextra -Wpedantic
            -Wno-unused-parameter
            -Wno-missing-field-initializers
        )
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        target_compile_options(${target_name} PRIVATE
            -Wall -Wextra -Wpedantic
            -Wno-unused-parameter
            -Wno-missing-field-initializers
            -Wno-gnu-zero-variadic-macro-arguments
        )
    elseif(MSVC)
        target_compile_options(${target_name} PRIVATE
            /W3
            /wd4996  # 禁用已弃用函数警告
            /wd4267  # 禁用size_t转换警告
            /wd4244  # 禁用类型转换警告
        )
    endif()
endfunction()

# 应用警告配置到所有库目标
if(BUILD_SHARED_LIBS)
    configure_nut_warnings(nut_shared)
endif()
if(BUILD_STATIC_LIBS)
    configure_nut_warnings(nut_static)
endif()

# 输出库信息
message(STATUS "NUT Library Configuration:")
if(BUILD_SHARED_LIBS)
    message(STATUS "  Building: Shared Library")
endif()
if(BUILD_STATIC_LIBS)
    message(STATUS "  Building: Static Library")
endif()
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  Sources: ${SOURCE_COUNT} files")
message(STATUS "  Headers: ${HEADER_COUNT} files")
message(STATUS "  Install Path: ${CMAKE_INSTALL_LIBDIR}")

