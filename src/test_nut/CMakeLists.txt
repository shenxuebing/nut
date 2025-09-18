# TEST_NUT 测试程序 CMakeLists.txt

# 收集所有测试源文件
file(GLOB_RECURSE TEST_NUT_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
)

# 收集所有测试头文件
file(GLOB_RECURSE TEST_NUT_HEADERS 
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

# 打印找到的文件数量
list(LENGTH TEST_NUT_SOURCES TEST_SOURCE_COUNT)
list(LENGTH TEST_NUT_HEADERS TEST_HEADER_COUNT)
message(STATUS "Found ${TEST_SOURCE_COUNT} test source files and ${TEST_HEADER_COUNT} test header files")

# 创建测试可执行文件
add_executable(test_nut ${TEST_NUT_SOURCES} ${TEST_NUT_HEADERS})

# 设置可执行文件属性
set_target_properties(test_nut PROPERTIES
    OUTPUT_NAME "test_nut"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# 让测试可执行遵循根 CMake 的 BUILD_RUNTIME 输出结构（Windows 下）
if(WIN32)
    if(NOT DEFINED BUILD_RUNTIME)
      set(BUILD_RUNTIME "MD" CACHE STRING "Choose runtime dir for outputs: MD or MT (Windows only)")
    endif()
    set_target_properties(test_nut PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${BUILD_RUNTIME}/bin")
endif()

# 包含目录
target_include_directories(test_nut 
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/..
        ${CMAKE_SOURCE_DIR}/src
)

# 链接nut库
target_link_libraries(test_nut 
    PRIVATE 
        nut
)

# 平台特定的链接库
if(PLATFORM_LINUX)
    target_link_libraries(test_nut 
        PRIVATE 
            Threads::Threads
    )
    
    # 链接原子操作库
    if(ATOMIC_LIBRARY)
        target_link_libraries(test_nut PRIVATE ${ATOMIC_LIBRARY})
    endif()
    
elseif(PLATFORM_DARWIN)
    # macOS 特定设置 
    target_link_libraries(test_nut PRIVATE "-framework Foundation")
    
elseif(PLATFORM_WINDOWS)
    # Windows 特定设置
    if(MSVC)
        # MSVC运行时库设置，与nut库保持一致
        set_property(TARGET test_nut PROPERTY
            MSVC_RUNTIME_LIBRARY ${CMAKE_MSVC_RUNTIME_LIBRARY}
        )
    endif()
endif()

# C++标准库链接
if(NOT PLATFORM_DARWIN AND NOT PLATFORM_WINDOWS)
    target_link_libraries(test_nut PRIVATE stdc++)
elseif(PLATFORM_DARWIN)
    target_link_libraries(test_nut PRIVATE c++)
endif()

# 编译警告设置
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(test_nut PRIVATE
        -Wall -Wextra
        -Wno-unused-parameter
        -Wno-missing-field-initializers
    )
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(test_nut PRIVATE
        -Wall -Wextra
        -Wno-unused-parameter
        -Wno-missing-field-initializers
        -Wno-gnu-zero-variadic-macro-arguments
    )
elseif(MSVC)
    target_compile_options(test_nut PRIVATE
        /W3
        /wd4996  # 禁用已弃用函数警告
        /wd4267  # 禁用size_t转换警告
        /wd4244  # 禁用类型转换警告
    )
endif()

# 为调试版本添加调试信息
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        target_compile_options(test_nut PRIVATE -g3 -ggdb)
    endif()
endif()

# 添加测试运行目标
add_custom_target(run_tests
    COMMAND $<TARGET_FILE:test_nut>
    DEPENDS test_nut
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running nut unit tests"
)

# 如果安装了Valgrind，添加内存检查目标（仅Linux）
if(PLATFORM_LINUX)
    find_program(VALGRIND_EXECUTABLE valgrind)
    if(VALGRIND_EXECUTABLE)
        add_custom_target(valgrind_tests
            COMMAND ${VALGRIND_EXECUTABLE} 
                --tool=memcheck 
                --leak-check=full 
                --show-leak-kinds=all 
                --track-origins=yes 
                --verbose 
                $<TARGET_FILE:test_nut>
            DEPENDS test_nut
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Running nut unit tests with Valgrind"
        )
        message(STATUS "Valgrind found: ${VALGRIND_EXECUTABLE}")
    else()
        message(STATUS "Valgrind not found, memory checking target will not be available")
    endif()
endif()

# 可选安装测试程序
option(INSTALL_TESTS "Install test executables" OFF)
if(INSTALL_TESTS)
    install(TARGETS test_nut
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
endif()

# 输出测试程序信息
message(STATUS "Test Program Configuration:")
message(STATUS "  Executable Name: test_nut")
message(STATUS "  Test Sources: ${TEST_SOURCE_COUNT} files")
message(STATUS "  Test Headers: ${TEST_HEADER_COUNT} files")
message(STATUS "  Install Tests: ${INSTALL_TESTS}")
if(PLATFORM_LINUX AND VALGRIND_EXECUTABLE)
    message(STATUS "  Valgrind Available: YES")
else()
    message(STATUS "  Valgrind Available: NO")
endif()

