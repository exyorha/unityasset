cmake_minimum_required(VERSION 3.20)
project(UnityAsset)

if(NOT TARGET lz4)
    find_package(PkgConfig REQUIRED)

    pkg_check_modules(LZ4 REQUIRED IMPORTED_TARGET liblz4)
    add_library(lz4 ALIAS PkgConfig::LZ4)
endif()

if(NOT MINGW)
    find_package(LibLZMA)
endif()

if(MINGW)
    set(ZLIB_USE_STATIC_LIBS ON)
endif()

find_package(ZLIB REQUIRED)

find_program(RUBY ruby REQUIRED)

add_subdirectory(UnitySerialization)
add_subdirectory(UnityContent)
add_subdirectory(ExtractUnityTypeData)
