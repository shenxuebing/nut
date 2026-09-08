cmake_minimum_required(VERSION 3.10)
project(libiconv C)

# 鸿蒙交叉编译设置
set(CMAKE_SYSTEM_NAME Ohos)

# 编译选项
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -O2")

# 定义 HAVE_CONFIG_H (使用简化配置)
add_definitions(-DHAVE_CONFIG_H)
add_definitions(-DLIBDIR="/usr/lib")
add_definitions(-DENABLE_RELOCATABLE=1)
add_definitions(-DIN_LIBRARY)

# 源文件
set(ICONV_SOURCES
    lib/iconv.c
    lib/relocatable.c
)

# 生成 config.h
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/config.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/config.h
)

# 包含目录
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}
)

# 创建静态库
add_library(iconv STATIC ${ICONV_SOURCES})

# 设置输出名称
set_target_properties(iconv PROPERTIES
    OUTPUT_NAME "iconv"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib/ohos/${OHOS_ARCH}"
)

# 安装规则
install(TARGETS iconv ARCHIVE DESTINATION lib)
install(FILES include/iconv.h DESTINATION include)