cmake_minimum_required(VERSION 3.20)
project(UnityAsset)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LZ4 REQUIRED IMPORTED_TARGET liblz4)

find_package(ZLIB REQUIRED)

add_library(UnityAsset STATIC
    include/UnityAsset/FileContainer/AssetBundle/AssetBundleBlock.h
    FileContainer/AssetBundle/AssetBundleBlock.cpp

    include/UnityAsset/FileContainer/AssetBundle/AssetBundleDirectory.h
    FileContainer/AssetBundle/AssetBundleDirectory.cpp

    include/UnityAsset/FileContainer/AssetBundle/AssetBundleDirectoryEntry.h
    FileContainer/AssetBundle/AssetBundleDirectoryEntry.cpp

    include/UnityAsset/FileContainer/AssetBundle/AssetBundleEntry.h
    FileContainer/AssetBundle/AssetBundleEntry.cpp

    include/UnityAsset/FileContainer/AssetBundle/AssetBundleFile.h
    FileContainer/AssetBundle/AssetBundleFile.cpp

    include/UnityAsset/Streams/FileInputOutput.h
    Streams/FileInputOutput.cpp

    include/UnityAsset/Streams/InMemoryStreamBackingBuffer.h
    Streams/InMemoryStreamBackingBuffer.cpp

    include/UnityAsset/Streams/MappedFileStreamBackingBuffer.h
    Streams/MappedFileStreamBackingBuffer.cpp

    include/UnityAsset/Streams/Stream.h
    Streams/Stream.cpp

    include/UnityAsset/Streams/StreamBackingBuffer.h
    Streams/StreamBackingBuffer.cpp

    include/UnityAsset/FileDescriptor.h
    FileDescriptor.cpp

    include/UnityAsset/UnityCompression.h
    UnityCompression.cpp
)

if(WIN32)
    target_sources(UnityAsset PRIVATE
        include/UnityAsset/WindowsError.h
        WindowsError.cpp

        include/UnityAsset/WindowsHandle.h
        WindowsHandle.cpp
    )
    target_compile_options(UnityAsset PRIVATE -DWIN32_LEAN_AND_MEAN -D_VC_EXTRALEAN -DNOMINMAX -DUNICODE -D_UNICODE)
endif()

target_include_directories(UnityAsset PUBLIC include)

set_target_properties(UnityAsset PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED TRUE
)
target_link_libraries(UnityAsset PRIVATE PkgConfig::LZ4 ZLIB::ZLIB)

if(NOT MSVC)
    target_compile_options(UnityAsset PRIVATE -Wall -W)
endif()

add_executable(assettest assettest.cpp)
set_target_properties(assettest PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED TRUE
)
target_link_libraries(assettest PRIVATE UnityAsset)
