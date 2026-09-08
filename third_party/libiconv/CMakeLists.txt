cmake_minimum_required(VERSION 3.15)

project(nut_bundled_libiconv C)

set(LIBICONV_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/source")

if(WIN32)
    set(LIBICONV_CONFIG_INCLUDE_DIRS
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        "${LIBICONV_SOURCE_DIR}"
    )
else()
    set(LIBICONV_GENERATED_DIR "${CMAKE_CURRENT_SOURCE_DIR}/generated/linux")
    set(LIBICONV_CONFIG_INCLUDE_DIRS
        "${LIBICONV_GENERATED_DIR}/include"
        "${LIBICONV_GENERATED_DIR}"
        "${LIBICONV_GENERATED_DIR}/lib"
        "${LIBICONV_GENERATED_DIR}/libcharset"
    )
endif()

add_library(nut_iconv_objects OBJECT
    "${LIBICONV_SOURCE_DIR}/lib/iconv.c"
    "${LIBICONV_SOURCE_DIR}/libcharset/lib/localcharset.c"
    "${LIBICONV_SOURCE_DIR}/libcharset/lib/relocatable-stub.c"
)

add_library(nut::iconv_objects ALIAS nut_iconv_objects)

target_include_directories(nut_iconv_objects
    PRIVATE
        ${LIBICONV_CONFIG_INCLUDE_DIRS}
        "${LIBICONV_SOURCE_DIR}/include"
        "${LIBICONV_SOURCE_DIR}/lib"
        "${LIBICONV_SOURCE_DIR}/libcharset/include"
)

target_compile_definitions(nut_iconv_objects
    PRIVATE
        HAVE_CONFIG_H
        IN_LIBRARY
        USING_STATIC_LIBICONV
        LIBDIR="/usr/lib"
)

set_target_properties(nut_iconv_objects PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)

if(MSVC)
    target_compile_options(nut_iconv_objects PRIVATE
        /wd4018
        /wd4090
        /wd4117
        /wd4244
        /wd4267
        /wd4273
        /wd4311
        /wd4996
    )
else()
    target_compile_options(nut_iconv_objects PRIVATE -Wno-unused-parameter)
endif()
